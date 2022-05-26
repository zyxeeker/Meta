#ifndef SRC_THREAD_POOL_H_
#define SRC_THREAD_POOL_H_

#include <iostream>
#include <list>
#include <memory>

#include "locker.h"
#include "task.h"

namespace thread {

class Pool {
 public:
  enum Result { ADD_REQUEST_SUCCESS, REQUEST_QUEUE_IS_FULL };

  Pool(size_t _max_request_num, size_t _max_thread_num);
  ~Pool();

  Result Append(Task* task);
  static void* Worker(void* arg);
  void Run();

 private:
  bool m_stop = false;

  std::unique_ptr<Locker> m_locker;
  std::unique_ptr<Sem> m_sem;
  // 线程队列
  pthread_t* m_thread_queue;
  // 线程数量最大值
  size_t m_max_thread_num;
  // 请求队列
  std::list<Task*> m_request_queue;
  // 请求数量最大值
  size_t m_max_request_num;
};

}  // namespace thread

#endif  // SRC_THREAD_POOL_H_
