#ifndef SRC_NET_OBJECT_H_
#define SRC_NET_OBJECT_H_

#include <memory>

#include "component/epoll.h"
#include "component/http_packet.h"
#include "component/http_parse.h"

namespace net {

struct Object {
  int fd;
  com::http::HttpCode http_code = com::http::OK;
  static std::shared_ptr<com::Epoll> epoll;

  std::shared_ptr<com::http::Parse> parse_handler;
  std::shared_ptr<com::http::Packet> packet_handler;

  // 上一次已读指针偏移
  size_t prev_reaed_offset = 0;
  // 已读指针偏移
  size_t readed_offset = 0;
  // 读区buffer
  char* readed_buf = nullptr;

  // 已写指针偏移
  size_t write_offset = 0;
  // 缓冲大小
  size_t buf_size = 0;
  // 文件大小
  size_t file_size = 0;
  // 分散读写区块
  struct iovec iov[2];
  // 写区buffer
  char* write_buf = nullptr;

  Object();
  ~Object();
  void Init();
};

}  // namespace net

#endif  // SRC_NET_OBJECT_H_
