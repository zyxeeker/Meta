#include "worker_io.h"

#include <sys/socket.h>
#include <sys/time.h>

#include <iostream>

#include "component/config.h"
#include "component/epoll.h"
#include "component/http.h"
#include "component/http_packet.h"
#include "component/http_parse.h"
#include "net/core.h"

namespace worker {
namespace io {

static size_t k_buffer_size = com::Config::Instance()->config().read_buf_length;

void Reader::Process() {
  // 请求体过大
  if (m_object->readed_offset >= k_buffer_size) {
    m_info->http_code = com::http::REQUEST_ENTITY_TOO_LARGE;
    m_info->epoll->Mod(m_info->fd, EPOLLOUT);
    return;
  }

  int readed = 0;
  while (1) {
    // 请求体过大
    if (m_object->readed_offset >= k_buffer_size) {
      m_info->http_code = com::http::REQUEST_ENTITY_TOO_LARGE;
      m_info->epoll->Mod(m_info->fd, EPOLLOUT);
      return;
    }

    readed = recv(m_info->fd, m_object->readed_buf + m_object->readed_offset,
                  k_buffer_size - m_object->readed_offset - 1, 0);
    // 出错
    if (readed == -1) {
      //
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        auto res = m_info->parse_handler->Process(m_object->readed_offset -
                                                  m_object->prev_reaed_offset);
        m_object->prev_reaed_offset = m_object->readed_offset;

        if (res == -1) break;
        m_info->http_code = (com::http::HttpCode)res;
        // 主业务处理加入到线程池
        m_object->to_pool_cb();
        return;
      }
      close(m_info->fd);
      return;
    }
    /* When a stream socket peer has performed an orderly shutdown, the return
     * value will be 0*/
    else if (readed == 0) {
      m_info->http_code = com::http::REQUEST_ENTITY_TOO_LARGE;
      m_info->epoll->Mod(m_info->fd, EPOLLOUT);
      return;
    }

    m_object->readed_offset += readed;
  }
  m_info->epoll->Mod(m_info->fd, EPOLLIN);
  return;
}

void Writer::Process() {
  auto type = m_info->packet_handler->type();
  const char* buf_p = nullptr;
  iovec* file_buf_p = nullptr;
  size_t buf_size = 0, file_size = 0;

  buf_p = m_info->packet_handler->buf();
  buf_size = m_info->packet_handler->buf_size();

  if (type == com::http::Packet::FILE) {
    file_buf_p = m_info->packet_handler->file_buf();
    file_size = m_info->packet_handler->file_size();
  }

  if (m_object->write_offset >= buf_size + file_size) return;

  int writed = 0;
  size_t total_size = buf_size + file_size;

  while (1) {
    if (type == com::http::Packet::CODE)
      writed = send(m_info->fd, buf_p + m_object->write_offset,
                    buf_size - m_object->write_offset, MSG_NOSIGNAL);
    else {
      int over = 0;
      // 使用iovec需自己计算偏移值
      if (m_object->write_offset >= buf_size) {
        over = m_object->write_offset - buf_size;
        file_buf_p[0].iov_len = 0;
        file_buf_p[1].iov_base = (char*)file_buf_p[1].iov_base + over;
        file_buf_p[1].iov_len = file_buf_p[1].iov_len - over;
      } else {
        file_buf_p[0].iov_base =
            (char*)file_buf_p[0].iov_base + m_object->write_offset;
        file_buf_p[0].iov_len = file_buf_p[0].iov_len - m_object->write_offset;
      }
      writed = writev(m_info->fd, file_buf_p, 2);
      m_object->write_offset = 0;
    }

    if (writed < 0) {
      if (errno = EAGAIN) {
        m_info->epoll->Mod(m_info->fd, EPOLLOUT);
        return;
      }
      m_object->reset_cb();
      close(m_info->fd);
      return;
    }
    if (writed == 0) {
      // 重置处理进程的成员
      m_object->reset_cb();
      // 释放打开文件
      m_info->packet_handler->ReleaseFile();
      // 等待下一次数据到达
      m_info->epoll->Mod(m_info->fd, EPOLLIN);
      return;
    }
    m_object->write_offset += writed;
  }
}

}  // namespace io
}  // namespace worker