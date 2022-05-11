#include "epoll.h"

#include "common/util.h"

namespace com {

Epoll::Epoll(int _max_events) : m_max_events(_max_events) {
  m_fd = epoll_create1(0);
  m_events = new epoll_event[m_max_events];
}

Epoll::~Epoll() { delete[] m_events; }

void Epoll::Add(int& fd) {
  epoll_event event;
  event.data.fd = fd;
  // TODO(zyxeeker): 增加 EPOLLLT 和 EPOLLET 的选择
  event.events = EPOLLIN | EPOLLET;

  epoll_ctl(m_fd, EPOLL_CTL_ADD, fd, &event);

  // set NOBLOCK
  util::SetNoBlock(fd);
}

void Epoll::Mod(int& fd) {
  // TODO(zyxeeker): ...
}

void Epoll::Del(int& fd) { epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, 0); }

int Epoll::Wait() { return epoll_wait(m_fd, m_events, 10, 0); }

}  // namespace com