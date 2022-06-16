#ifndef SRC_COMPONENT_EPOLL_H_
#define SRC_COMPONENT_EPOLL_H_

#include <sys/epoll.h>

#include "common/util.h"

namespace com {

class EpollEventManager {
 public:
  EpollEventManager(int _max_events_num)
      : m_events(new epoll_event[_max_events_num]),
        m_max_event_num(_max_events_num) {}
  EpollEventManager() { delete[] m_events; }

  int Wait(int epoll_fd) {
    return epoll_wait(epoll_fd, m_events, m_max_event_num, 0);
  }

  epoll_event* events(int& i) const { return m_events + i; }

 private:
  int m_max_event_num = 10;
  epoll_event* m_events = nullptr;
};

static int CreateEpoll(int flags = 0) { return epoll_create1(flags); }

static void EpollAddFd(int epoll_fd, int fd, bool et_mode,
                       bool one_shot = false) {
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN;

  if (et_mode) event.events |= EPOLLET;
  if (one_shot) event.events |= EPOLLONESHOT;

  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);

  // set NOBLOCK
  util::SetNoBlock(fd);
}

static void EpollDelFd(int epoll_fd, int fd) {
  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0);
}

static void EpollModFd(int epoll_fd, int fd, int op) {
  epoll_event event;
  event.data.fd = fd;
  event.events = op | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;

  epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
}

}  // namespace com

#endif  // SRC_COMPONENT_EPOLL_H_
