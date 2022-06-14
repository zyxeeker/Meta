#include "worker_handler.h"

#include "component/config.h"
#include "logger/core.h"
#include "thread/thread.h"
#include "worker_waiter.h"

namespace worker {
namespace io {

static size_t k_buffer_size = com::Config::Instance()->config().read_buf_length;

pthread_cond_t ReaderHandler::m_k_cond = PTHREAD_COND_INITIALIZER;

pthread_cond_t WriterHandler::m_k_cond = PTHREAD_COND_INITIALIZER;

pthread_cond_t ContentHandler::m_k_cond = PTHREAD_COND_INITIALIZER;

void ReaderHandler::Process() {
  struct thread::Job* job = nullptr;
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  while (1) {
    pthread_mutex_lock(&lock);
    // 条件判断
    while (!(job = WorkerWaiter::Instance()->GetObject(WorkerWaiter::READER))) {
      pthread_cond_wait(&m_k_cond, &lock);
    }
    // 处理区
    job->run(job->arg);

    pthread_mutex_unlock(&lock);
  }
}

void WriterHandler::Process() {
  struct thread::Job* job = nullptr;
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  while (1) {
    pthread_mutex_lock(&lock);
    // 条件判断
    while (!(job = WorkerWaiter::Instance()->GetObject(WorkerWaiter::WRITER))) {
      pthread_cond_wait(&m_k_cond, &lock);
    }
    // 处理区
    job->run(job->arg);

    delete job;

    pthread_mutex_unlock(&lock);
  }
}

void ContentHandler::Process() {
  struct thread::Job* job = nullptr;
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  while (1) {
    pthread_mutex_lock(&lock);
    // 条件判断
    while (
        !(job = WorkerWaiter::Instance()->GetObject(WorkerWaiter::CONTENT))) {
      pthread_cond_wait(&m_k_cond, &lock);
    }
    MINFO() << "DO IT: " << ((net::Object*)job->arg)->fd;
    // 处理区
    job->run(job->arg);

    delete job;

    pthread_mutex_unlock(&lock);
  }
}

}  // namespace io
}  // namespace worker