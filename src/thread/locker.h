#ifndef SRC_THREAD_LOCKER_H_
#define SRC_THREAD_LOCKER_H_

#include <pthread.h>
#include <semaphore.h>

#include <exception>

namespace thread {

class Locker {
 public:
  Locker() {
    if (pthread_mutex_init(&m_mutex, nullptr) != 0) throw std::exception();
  }
  ~Locker() { pthread_mutex_destroy(&m_mutex); }

  bool Lock() { return pthread_mutex_lock(&m_mutex) == 0; }
  bool Unlock() { return pthread_mutex_unlock(&m_mutex) == 0; }

 private:
  pthread_mutex_t m_mutex;
};

class Sem {
 public:
  Sem() {
    if (sem_init(&m_sem, 0, 0) != 0) throw std::exception();
  }
  ~Sem() { sem_destroy(&m_sem); }

  bool Wait() { return sem_wait(&m_sem) == 0; }
  bool Post() { return sem_post(&m_sem) == 0; }

 private:
  sem_t m_sem;
};

class Cond {
 public:
  Cond() {
    if (pthread_mutex_init(&m_mutex, nullptr) != 0) throw std::exception();
    if (pthread_cond_init(&m_cond, nullptr) != 0) {
      pthread_mutex_destroy(&m_mutex);
      throw std::exception();
    }
  }
  ~Cond() {
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
  }

  bool Wait() {
    int ret = 0;
    pthread_mutex_lock(&m_mutex);
    ret = pthread_cond_wait(&m_cond, &m_mutex);
    pthread_mutex_unlock(&m_mutex);
    return ret == 0;
  }
  bool Signal() { return pthread_cond_signal(&m_cond); }

 private:
  pthread_mutex_t m_mutex;
  pthread_cond_t m_cond;
};

}  // namespace thread

#endif  // SRC_THREAD_LOCKER_H_
