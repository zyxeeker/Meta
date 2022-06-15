#include "deliver_core.h"

#include <sys/epoll.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

#include "common/util.h"
#include "logger/core.h"

#define BUFFER_LEN 409600

namespace net {

static int num = 0;

DeliverCore::DeliverCore(const char* addr, int port, Object* obj) : m_obj(obj) {
  int ad = inet_addr(addr);

  struct sockaddr_in address;
  bzero(&m_addr, sizeof(m_addr));
  m_addr.sin_family = AF_INET;
  m_addr.sin_port = htons(port);
  m_addr.sin_addr.s_addr = ad;
}

void DeliverCore::Init() {
  if (m_socket != -1) return;
  m_socket = socket(AF_INET, SOCK_STREAM, 0);
  int res = connect(m_socket, (sockaddr*)&m_addr, sizeof(m_addr));

  printf("SOCK: %d\n", m_socket);
  // TODO
  if (res < 0) {
    printf("RES: %d\n", res);
    return;
  }
  // set NOBLOCK
  util::SetNoBlock(m_socket);
}

void DeliverCore::Close() {
  m_run = false;
  close(m_socket);
}

void DeliverCore::Process() {
  MDEBUG() << "BUFFER: " << m_obj->readed_buf;

  printf("---BEGIN---\n");
  send(m_socket, m_obj->readed_buf, m_obj->readed_offset, 0);
  m_obj->Init();

  int len = -1;
  epoll_event* m_events = new epoll_event[2];
  int offset = 0;

  char buffer[BUFFER_LEN];
  m_obj->packet_handler->SetTrans();

  while (m_run) {
    len = read(m_socket, buffer, BUFFER_LEN);

    if (len == 0) break;
    if (len < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        usleep(500);

      } else {
        printf("BREAK|\n");
        break;
      }
    } else {
      char* dst_buf = new char[len];
      memcpy(dst_buf, buffer, len);

      pthread_mutex_lock(&m_obj->chunk_lock);
      m_obj->chunk_data.push({dst_buf, len});
      pthread_mutex_unlock(&m_obj->chunk_lock);
      m_obj->epoll->Mod(m_obj->fd, EPOLLOUT);

      printf("LEN: %d\n", len);
    }
  }
  close(m_socket);
  printf("---END---\n");
}  // namespace net

}  // namespace net