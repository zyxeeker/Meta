#ifndef SRC_ALGO_EPOLL_H_
#define SRC_ALGO_EPOLL_H_

#include <sys/epoll.h>

namespace com {

class Epoll {
 public:
  Epoll(int _max_events);
  ~Epoll();

  void Add(int& fd, bool et_mode = true, bool one_shot = false);
  void Del(int& fd);
  void Mod(int& fd, int op);
  int Wait();

  int fd() const { return m_fd; }
  epoll_event* events(int& i) const { return m_events + i; }

 private:
  int m_fd;
  int m_max_events;
  epoll_event* m_events;
};

}  // namespace com

#endif  // SRC_ALGO_EPOLL_H_
