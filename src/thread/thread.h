#ifndef SRC_THREAD_THREAD_H_
#define SRC_THREAD_THREAD_H_

#include <pthread.h>
#include <signal.h>
#include <sys/types.h>

#include "logger/core.h"

namespace thread {

struct Job {
  void (*run)(void* arg);
  void* arg;
};

// 线程运行判断
static bool JudgeThreadAlive(pthread_t& thread) {
  if (thread == -1) return false;
  int kill_rc = pthread_kill(thread, 0);
  if (kill_rc == ESRCH)
    MDEBUG() << "The specified thread did not exists or already quit";
  else if (kill_rc == EINVAL)
    MDEBUG() << "The signal is invalid";
  else {
    MDEBUG() << "The specified thread is alive pthread\n";
    return true;
  }
  return false;
}

}  // namespace thread

#endif  // SRC_THREAD_THREAD_H_
