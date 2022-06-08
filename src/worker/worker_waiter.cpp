#include "worker_waiter.h"

#include "worker.h"
#include "worker_handler.h"

namespace worker {

WorkerWaiter* WorkerWaiter::inst = nullptr;

WorkerWaiter::WorkerWaiter() {
  m_reader_pool = new thread::Pool(32, io::ReaderHandler::Process);
  m_content_pool = new thread::Pool(64, io::ContentHandler::Process);
  m_write_pool = new thread::Pool(32, io::WriterHandler::Process);
}

thread::Job* WorkerWaiter::GetObject(int type) {
  pthread_mutex_lock(&m_lock);
  switch (type) {
    case READER: {
      if (!m_ready_to_read_count) break;
      auto p = m_read_queue.front();
      m_read_queue.pop();
      --m_ready_to_read_count;
      pthread_mutex_unlock(&m_lock);
      return p;
    }
    case CONTENT: {
      if (!m_ready_to_content_count) break;
      auto p = m_content_queue.front();
      m_content_queue.pop();
      --m_ready_to_content_count;
      pthread_mutex_unlock(&m_lock);
      return p;
    }
    case WRITER: {
      if (!m_ready_to_write_count) break;
      auto p = m_write_queue.front();
      m_write_queue.pop();
      --m_ready_to_write_count;
      pthread_mutex_unlock(&m_lock);
      return p;
    }
  }
  pthread_mutex_unlock(&m_lock);
  return nullptr;
}

void WorkerWaiter::Append(int type, net::Object* obj) {
  pthread_mutex_lock(&m_lock);
  switch (type) {
    case READER: {
      m_read_queue.push(new thread::Job{ReaderHandlerFun, obj});
      ++m_ready_to_read_count;
      pthread_cond_signal(&io::ReaderHandler::m_k_cond);
      break;
    }
    case CONTENT: {
      m_content_queue.push(new thread::Job{ContentHandlerFun, obj});
      ++m_ready_to_content_count;
      pthread_cond_signal(&io::ContentHandler::m_k_cond);
      break;
    }
    case WRITER: {
      m_write_queue.push(new thread::Job{WriterHandlerFun, obj});
      ++m_ready_to_write_count;
      pthread_cond_signal(&worker::io::WriterHandler::m_k_cond);
      break;
    }
  }
  pthread_mutex_unlock(&m_lock);
}

}  // namespace worker
