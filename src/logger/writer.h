#ifndef SRC_LOGGER_WRITER_H_
#define SRC_LOGGER_WRITER_H_

#include <pthread.h>
#include <semaphore.h>

#include <iostream>
#include <string>

#include "log.h"

namespace logger {

class WriterProducer {
 public:
  WriterProducer() { sem_init(&m_k_sem, 0, 0); }

  static void Start() { sem_post(&m_k_sem); }

  static void Process();

 public:
  static sem_t m_k_sem;
};

class WriterConsumer {
 public:
  WriterConsumer() { sem_init(&m_k_sem, 0, 1); }
  static void Process();

 public:
  static sem_t m_k_sem;
};

}  // namespace logger

#endif  // SRC_LOGGER_WRITER_H_
