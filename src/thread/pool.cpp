#include "pool.h"

namespace thread {

Pool::Pool(size_t _max_request_num, size_t _max_thread_num)
    : m_max_request_num(_max_request_num), m_max_thread_num(_max_thread_num) {
  m_locker.reset(new Locker());
  m_sem.reset(new Sem());
  // m_locker = std::make_unique<Locker>();
  // m_sem = std::make_unique<Sem>();
  m_thread_queue = new pthread_t[m_max_thread_num];

  for (int i = 0; i < m_max_thread_num; ++i) {
    if (pthread_create(m_thread_queue + i, nullptr, Worker, this)) {
      delete[] m_thread_queue;
      throw std::exception();
    }
    if (pthread_detach(m_thread_queue[i])) {
      delete[] m_thread_queue;
      throw std::exception();
    }
  }
}

Pool::~Pool() {
  delete[] m_thread_queue;
  // m_stop = true;
}

Pool::Result Pool::Append(Task* task) {
  if (m_request_queue.size() == m_max_request_num) return REQUEST_QUEUE_IS_FULL;

  m_locker->Lock();
  m_request_queue.push_back(task);
  m_locker->Unlock();
  m_sem->Post();

  return ADD_REQUEST_SUCCESS;
}

void* Pool::Worker(void* arg) {
  auto pool = (Pool*)arg;
  pool->Run();
  return pool;
}

void Pool::Run() {
  while (!m_stop) {
    m_sem->Wait();
    m_locker->Lock();
    Task* task = m_request_queue.front();
    m_request_queue.pop_front();
    m_locker->Unlock();

    if (!task) continue;

    task->Process();
  }
}

}  // namespace thread