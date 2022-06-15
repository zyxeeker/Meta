#ifndef SRC_NET_OBJECT_H_
#define SRC_NET_OBJECT_H_

#include <pthread.h>

#include <list>
#include <memory>
#include <queue>

#include "component/epoll.h"
#include "component/http_packet.h"
#include "component/http_parse.h"

namespace net {

class DeliverCore;

struct Chunk {
  char* addr = nullptr;
  char* addr_offset = nullptr;
  size_t size = 0;

  Chunk(char* p, size_t l) : addr(p), addr_offset(p), size(l) {}
  ~Chunk() {
    if (!addr) {
      printf("NOT NULL!\n");
      delete[] addr;
    }
  }
};

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

  // 转发数据
  std::queue<Chunk> chunk_data;

  pthread_mutex_t chunk_lock = PTHREAD_MUTEX_INITIALIZER;
  bool transfer_thread_exist = false;
  pthread_t transfer_thread;
  pthread_t* transfer_read_thread = nullptr;

  DeliverCore* deliver = nullptr;

  Object();
  ~Object();
  void Init();
  void Close();
};

}  // namespace net

#endif  // SRC_NET_OBJECT_H_
