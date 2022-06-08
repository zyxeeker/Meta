#include "worker_handler.h"

#include "component/config.h"
#include "thread/thread.h"
#include "worker_waiter.h"

namespace worker {
namespace io {

static size_t k_buffer_size = com::Config::Instance()->config().read_buf_length;

pthread_mutex_t ReaderHandler::m_k_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ReaderHandler::m_k_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t WriterHandler::m_k_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t WriterHandler::m_k_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t ContentHandler::m_k_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ContentHandler::m_k_cond = PTHREAD_COND_INITIALIZER;

void ReaderHandler::Process() {
  struct thread::Job* job = nullptr;
  while (1) {
    pthread_mutex_lock(&m_k_lock);
    // 条件判断
    while (!(job = WorkerWaiter::Instance()->GetObject(WorkerWaiter::READER))) {
      pthread_cond_wait(&m_k_cond, &m_k_lock);
    }
    // 处理区
    job->run(job->arg);

    pthread_mutex_unlock(&m_k_lock);
  }
}

void WriterHandler::Process() {
  struct thread::Job* job = nullptr;
  while (1) {
    pthread_mutex_lock(&m_k_lock);
    // 条件判断
    while (!(job = WorkerWaiter::Instance()->GetObject(WorkerWaiter::WRITER))) {
      pthread_cond_wait(&m_k_cond, &m_k_lock);
    }
    // 处理区
    job->run(job->arg);

    delete job;

    pthread_mutex_unlock(&m_k_lock);
  }
}

void ContentHandler::Process() {
  struct thread::Job* job = nullptr;
  while (1) {
    pthread_mutex_lock(&m_k_lock);
    // 条件判断
    while (
        !(job = WorkerWaiter::Instance()->GetObject(WorkerWaiter::CONTENT))) {
      pthread_cond_wait(&m_k_cond, &m_k_lock);
    }
    // 处理区
    job->run(job->arg);

    delete job;

    pthread_mutex_unlock(&m_k_lock);
  }
}

}  // namespace io
}  // namespace worker