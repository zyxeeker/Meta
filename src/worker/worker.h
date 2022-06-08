#ifndef SRC_WORKER_WORKER_H_
#define SRC_WORKER_WORKER_H_

#include "component/config.h"
#include "component/epoll.h"
#include "component/http.h"
#include "component/router.h"
#include "net/object.h"
#include "worker_waiter.h"

namespace worker {

static size_t k_buffer_size = com::Config::Instance()->config().read_buf_length;

void ReaderHandlerFun(void* arg) {
  struct net::Object* object = (net::Object*)arg;

  if (object->readed_offset >= k_buffer_size) {
    object->http_code = com::http::REQUEST_ENTITY_TOO_LARGE;
    object->epoll->Mod(object->fd, EPOLLOUT);
    return;
  }

  int readed = 0;
  while (1) {
    // 请求体过大
    if (object->readed_offset >= k_buffer_size) {
      object->http_code = com::http::REQUEST_ENTITY_TOO_LARGE;
      object->epoll->Mod(object->fd, EPOLLOUT);
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
      object->epoll->Mod(object->fd, EPOLLOUT);
      return;
    }

    object->readed_offset += readed;
  }
  object->epoll->Mod(object->fd, EPOLLIN);
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
  }

  if (object->write_offset >= buf_size + file_size) return;

  int writed = 0;
  size_t total_size = buf_size + file_size;

  while (1) {
    if (type == com::http::Packet::CODE)
      writed = send(object->fd, buf_p + object->write_offset,
                    buf_size - object->write_offset, MSG_NOSIGNAL);
    else {
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
    }

    if (writed < 0) {
      if (errno = EAGAIN) {
        object->epoll->Mod(object->fd, EPOLLOUT);
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
      object->epoll->Mod(object->fd, EPOLLIN);
      return;
    }
    object->write_offset += writed;
  }
}

void ContentHandlerFun(void* arg) {
  struct net::Object* object = (net::Object*)arg;

  if (object->http_code != 200) {
    object->packet_handler->Process(object->http_code);
  } else {
    auto path = object->parse_handler->url();
    auto version = object->parse_handler->version();

    auto find_res = com::Router::Instance()->Find(path);

    if (!find_res)
      object->packet_handler->Process(com::http::NOT_FOUND);
    else
      object->packet_handler->Process(path, com::http::Packet::FILE);
  }
  object->epoll->Mod(object->fd, EPOLLOUT);
}

}  // namespace worker

#endif  // SRC_WORKER_WORKER_H_
