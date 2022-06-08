#ifndef SRC_WORKER_WORKER_WAITER_H_
#define SRC_WORKER_WORKER_WAITER_H_

#include <queue>

#include "net/object.h"
#include "thread/abstract_waiter.h"
#include "thread/pool.h"

namespace worker {

class WorkerWaiter : public thread::AbstractWaiter {
 public:
  enum Type { READER = 0, CONTENT, WRITER };
  static WorkerWaiter* Instance() {
    if (!inst) inst = new WorkerWaiter();
    return inst;
  }
  WorkerWaiter();

  thread::Job* GetObject(int type) override;
  void Append(int type, net::Object* obj);

 private:
  static WorkerWaiter* inst;

  pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;

  std::queue<thread::Job*> m_read_queue;
  std::queue<thread::Job*> m_content_queue;
  std::queue<thread::Job*> m_write_queue;

  int m_ready_to_read_count = 0;
  int m_ready_to_content_count = 0;
  int m_ready_to_write_count = 0;

  thread::Pool* m_reader_pool = nullptr;
  thread::Pool* m_content_pool = nullptr;
  thread::Pool* m_write_pool = nullptr;
};

}  // namespace worker

#endif  // SRC_WORKER_WORKER_WAITER_H_
