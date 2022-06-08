#ifndef SRC_WORKER_WORKER_HANDLER_H_
#define SRC_WORKER_WORKER_HANDLER_H_

#include <pthread.h>
#include <semaphore.h>

#include "net/object.h"

namespace worker {
namespace io {

class ReaderHandler {
 public:
  static void Process();

 public:
  static pthread_mutex_t m_k_lock;
  static pthread_cond_t m_k_cond;
};

class WriterHandler {
 public:
  static void Process();

 public:
  static pthread_mutex_t m_k_lock;
  static pthread_cond_t m_k_cond;
};

class ContentHandler {
 public:
  static void Process();

 public:
  static pthread_mutex_t m_k_lock;
  static pthread_cond_t m_k_cond;
};

}  // namespace io
}  // namespace worker

#endif  // SRC_WORKER_WORKER_HANDLER_H_
