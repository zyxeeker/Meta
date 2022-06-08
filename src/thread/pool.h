#ifndef SRC_THREAD_POOL_H_
#define SRC_THREAD_POOL_H_

#include <pthread.h>
#include <sys/types.h>

namespace thread {

class Pool {
 public:
  struct thread {
    int id;
    pthread_t thread;
    void (*enty)();
    void* arg;
  };

  Pool(u_int32_t _cap, void (*fn)());
  ~Pool();

  static void* Wrapper(void* arg);
  void Join();

 private:
  u_int32_t m_capactity;
  struct thread* m_pool = nullptr;
};

}  // namespace thread

#endif  // SRC_THREAD_POOL_H_
