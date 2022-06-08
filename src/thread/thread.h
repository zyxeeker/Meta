#ifndef SRC_THREAD_THREAD_H_
#define SRC_THREAD_THREAD_H_

#include <pthread.h>
#include <sys/types.h>

#include <cstdio>

namespace thread {

struct Job {
  void (*run)(void* arg);
  void* arg;
};

}  // namespace thread

#endif  // SRC_THREAD_THREAD_H_
