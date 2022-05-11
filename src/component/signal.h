#ifndef SRC_COMPONENT_SIGNAL_H_
#define SRC_COMPONENT_SIGNAL_H_

#include <sys/signal.h>

namespace com {

class Signal {
 public:
  Signal(int type) : m_type(type){};
  ~Signal() { delete[] m_pipe; }

  int Init();
  // 返回读端给使用线程
  int& read_fd() { return m_pipe[0]; }

 private:
  // 信号重定向后触发的函数
  static void Handler(int sig);

 private:
  int m_type;
  // 创建pipe 用来和使用线程通信
  static int* m_pipe;
};

}  // namespace com

#endif  // SRC_COMPONENT_SIGNAL_H_
