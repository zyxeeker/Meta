#ifndef SRC_WORKER_WORKER_H_
#define SRC_WORKER_WORKER_H_

#include <functional>
#include <memory>

#include "component/epoll.h"
#include "component/http_packet.h"
#include "component/http_parse.h"

namespace worker {

typedef struct info {
  int fd;
  com::http::HttpCode http_code = com::http::OK;
  std::shared_ptr<com::Epoll> epoll;
  std::shared_ptr<com::http::Parse> parse_handler;
  std::shared_ptr<com::http::Packet> packet_handler;
} Info;

typedef struct reader_object {
  // 上一次已读指针偏移
  size_t prev_reaed_offset = 0;
  // 已读指针偏移
  size_t readed_offset = 0;
  // 把父类加进处理线程池的回调函数
  std::function<void()> to_pool_cb;
  // 读区buffer
  char* readed_buf = nullptr;
} ReaderObject;

typedef struct writer_object {
  // 已写指针偏移
  size_t write_offset = 0;
  // 缓冲大小
  size_t buf_size = 0;
  // 文件大小
  size_t file_size = 0;
  // 重置父类的数据成员的回调函数
  std::function<void()> reset_cb;

  struct iovec iov[2];
  // 写区buffer
  char* write_buf = nullptr;
} WriterObject;

}  // namespace worker

#endif  // SRC_WORKER_WORKER_H_
