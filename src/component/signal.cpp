#include "signal.h"

#include <sys/socket.h>

#include <cstring>

#include "common/util.h"

namespace com {

int* Signal::m_pipe = new int[2];

int Signal::Init() {
  // socketpair 比 pipe 函数创建方便，可以双向读写
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, m_pipe) == -1) return -1;
  util::SetNoBlock(m_pipe[1]);

  struct sigaction sa;
  bzero(&sa, sizeof(sa));

  /* the call is automatically restarted after the signal handler returns if the
   * SA_RESTART flag was used */
  sa.sa_flags |= SA_RESTART;
  sa.sa_handler = Handler;
  sigfillset(&sa.sa_mask);

  // use sigaction() to change signal`s behavior in progress
  return sigaction(m_type, &sa, nullptr);
}

void Signal::Handler(int sig) { send(m_pipe[1], (char*)&sig, 1, 0); }

}  // namespace com
