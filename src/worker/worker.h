#ifndef SRC_WORKER_WORKER_H_
#define SRC_WORKER_WORKER_H_

#include <signal.h>

#include "component/config.h"
#include "component/epoll.h"
#include "component/http.h"
#include "component/router.h"
#include "logger/core.h"
#include "net/object.h"
#include "worker_proxy.h"
#include "worker_waiter.h"

#define BUFFER_LEN 409600

namespace worker {

static size_t k_buffer_size = com::Config::Instance()->config().read_buf_length;

void ReaderHandlerFun(void* arg) {
  struct net::Object* object = (net::Object*)arg;

  if (object->readed_offset >= k_buffer_size) {
    object->http_code = com::http::REQUEST_ENTITY_TOO_LARGE;
    object->EpollMod();
    return;
  }

  int readed = 0;
  while (1) {
    // 请求体过大
    if (object->readed_offset >= k_buffer_size) {
      object->http_code = com::http::REQUEST_ENTITY_TOO_LARGE;
      object->EpollMod();
      return;
    }

    readed = recv(object->fd, object->readed_buf + object->readed_offset,
                  k_buffer_size - object->readed_offset - 1, 0);
    // 出错
    if (readed == -1) {
      //
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        auto res = object->parse_handler->Process(object->readed_offset -
                                                  object->prev_reaed_offset);
        object->prev_reaed_offset = object->readed_offset;

        if (res == -1) break;
        object->http_code = (com::http::HttpCode)res;

        MDEBUG() << "READER BUFFER: " << object->readed_buf;
        // 主业务处理加入到线程池
        WorkerWaiter::Instance()->Append(WorkerWaiter::CONTENT, object);
        return;
      }
      close(object->fd);
      return;
    }
    /* When a stream socket peer has performed an orderly shutdown, the return
       value will be 0*/
    else if (readed == 0) {
      object->http_code = com::http::REQUEST_ENTITY_TOO_LARGE;
      object->EpollMod();
      return;
    }

    object->readed_offset += readed;
  }
  object->EpollMod(EPOLLIN);
  return;
}

void WriterHandlerFun(void* arg) {
  struct net::Object* object = (net::Object*)arg;

  auto type = object->packet_handler->type();
  const char* buf_p = nullptr;
  iovec* file_buf_p = nullptr;
  size_t buf_size = 0, file_size = 0;

  buf_p = object->packet_handler->buf();
  buf_size = object->packet_handler->buf_size();

  if (type == com::http::Packet::FILE) {
    file_buf_p = object->packet_handler->file_buf();
    file_size = object->packet_handler->file_size();
    if (object->write_offset >= buf_size + file_size) return;
  }

  int writed = 0;
  size_t total_size = buf_size + file_size;

  while (1) {
    switch (type) {
      case com::http::Packet::CODE: {
        writed = send(object->fd, buf_p + object->write_offset,
                      buf_size - object->write_offset, MSG_NOSIGNAL);
        break;
      }
      case com::http::Packet::FILE: {
        int over = 0;
        // 使用iovec需自己计算偏移值
        if (object->write_offset >= buf_size) {
          over = object->write_offset - buf_size;
          file_buf_p[0].iov_len = 0;
          file_buf_p[1].iov_base = (char*)file_buf_p[1].iov_base + over;
          file_buf_p[1].iov_len = file_buf_p[1].iov_len - over;
        } else {
          file_buf_p[0].iov_base =
              (char*)file_buf_p[0].iov_base + object->write_offset;
          file_buf_p[0].iov_len = file_buf_p[0].iov_len - object->write_offset;
        }
        writed = writev(object->fd, file_buf_p, 2);
        object->write_offset = 0;
        break;
      }
      case com::http::Packet::TRANS: {
        if (thread::JudgeThreadAlive(object->proxy_resp_thread)) return;

        pthread_create(&(object->proxy_resp_thread), nullptr,
                       ProxyRespHandlerFun, object);
        pthread_detach(object->proxy_resp_thread);

        return;
      }
    }

    if (writed < 0) {
      if (errno = EAGAIN) {
        object->EpollMod();
        return;
      }
      // 重新初始化处理线程的成员
      object->Init();
      close(object->fd);
      return;
    }
    if (writed == 0) {
      // 重新初始化处理线程的成员
      object->Init();
      // 释放打开文件
      object->packet_handler->ReleaseFile();
      // 等待下一次数据到达
      object->EpollMod(EPOLLIN);
      return;
    }
    object->write_offset += writed;
  }
}

// 内容处理
void ContentHandlerFun(void* arg) {
  struct net::Object* object = (net::Object*)arg;

  if (object->http_code != 200) {
    object->packet_handler->Process(object->http_code);
    object->EpollMod();
    return;
  }

  // proxy
  std::string p_host_value;
  com::Config::UrlRedirectConfig* p_url_cfg = nullptr;
  try {
    p_host_value = object->parse_handler->header().at("Host");
    MDEBUG() << "HOST: " << p_host_value.c_str();
    p_url_cfg =
        com::Config::Instance()->config().redirect_settings.at(p_host_value);
  } catch (...) {
    MDEBUG() << "TRANSFER HOST NOT FOUND!";
  }

  // proxy
  if (p_url_cfg) {
    // method拦截
    if ((p_url_cfg->allow_method & object->parse_handler->method()) == 0) {
      object->packet_handler->Process(com::http::METHOD_NOT_ALLOWED);
      object->EpollMod();
      return;
    }
    if (!object->InitProxy(p_url_cfg->dst.c_str(), p_url_cfg->port)) {
      object->packet_handler->Process(com::http::INTERNAL_SERVER_ERROR);
      object->EpollMod();
      return;
    }
    ProxyClinetSenderFun(object);
    ProxyClinetReaderFun(object);

    return;
  } else {
    auto path = object->parse_handler->url();
    auto version = object->parse_handler->version();

    auto find_res = com::Router::Instance()->Find(path);

    if (!find_res)
      object->packet_handler->Process(com::http::NOT_FOUND);
    else
      object->packet_handler->Process(path, com::http::Packet::FILE);
  }
  object->EpollMod();
}

}  // namespace worker

#endif  // SRC_WORKER_WORKER_H_
