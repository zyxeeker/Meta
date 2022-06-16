#ifndef SRC_WORKER_WORKER_PROXY_H_
#define SRC_WORKER_WORKER_PROXY_H_

#include "logger/core.h"
#include "net/object.h"

#define BUFFER_LEN 409600

namespace worker {

void ProxyClinetSenderFun(void* arg) {
  struct net::Object* object = (net::Object*)arg;

  int sended = 0;
  int p_socket = object->client->get_fd();
  char* p_buf_p = object->readed_buf;
  int p_buf_len = object->readed_offset;

  while (sended != p_buf_len) {
    auto res = send(p_socket, p_buf_p + sended, p_buf_len - sended, 0);
    if (res < 0) {
      usleep(500);
      continue;
    }
    sended += res;
  }
  object->Init();
  object->packet_handler->SetTrans();
}

void ProxyClinetReaderFun(void* arg) {
  struct net::Object* object = (net::Object*)arg;
  int p_socket = object->client->get_fd();

  int len = -1;
  int writed = 0;
  char buffer[BUFFER_LEN];

  while (true) {
    len = read(p_socket, buffer, BUFFER_LEN);

    if (len == 0)
      break;
    else if (len < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        usleep(500);
      else
        break;

    } else {
      char* dst_buf = new char[len];
      memcpy(dst_buf, buffer, len);

      pthread_mutex_lock(&(object->chunk_lock));
      object->chunk_data.push({dst_buf, len});
      pthread_mutex_unlock(&(object->chunk_lock));
      object->EpollMod();
    }
  }
  object->Close();
}

void* ProxyRespHandlerFun(void* arg) {
  struct net::Object* object = (net::Object*)arg;

  int writed = 0;
  while (object->chunk_data.size() > 0) {
    pthread_mutex_lock(&object->chunk_lock);

    auto& p = object->chunk_data.front();

    if (p.size == 0) {
      object->chunk_data.pop();
      pthread_mutex_unlock(&object->chunk_lock);
      continue;
    }

    writed = send(object->fd, p.addr_offset, p.size, MSG_NOSIGNAL);

    if (writed < 0) {
      if (errno = EAGAIN) {
        object->EpollMod();
        pthread_mutex_unlock(&object->chunk_lock);
        return nullptr;
      }
      // 重新初始化处理线程的成员
      object->Init();
      close(object->fd);
      break;
    }

    p.size -= writed;
    p.addr_offset += writed;

    pthread_mutex_unlock(&object->chunk_lock);
  }
  pthread_mutex_unlock(&object->chunk_lock);
  object->EpollMod(EPOLLIN);
}

}  // namespace worker

#endif  // SRC_WORKER_WORKER_PROXY_H_
