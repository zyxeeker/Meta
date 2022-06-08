#include "writer.h"

#include <unistd.h>

#include <cstring>
#include <fstream>

#include "core.h"
#include "thread/thread.h"

namespace logger {

static LogBuffer *k_buffer = nullptr;
static pthread_mutex_t k_lock = PTHREAD_MUTEX_INITIALIZER;

sem_t WriterProducer::m_k_sem{0};
sem_t WriterConsumer::m_k_sem{0};

void WriterProducer::Process() {
  LogBuffer *p = nullptr;
  while (1) {
    sem_wait(&m_k_sem);
    pthread_mutex_lock(&k_lock);

    k_buffer = (LogBuffer *)Core::Instance()->GetObj();

    sleep(1);

    pthread_mutex_unlock(&k_lock);
    sem_post(&WriterConsumer::m_k_sem);
  }
}

void WriterConsumer::Process() {
  LogBuffer *p = nullptr;
  while (1) {
    sem_wait(&m_k_sem);
    pthread_mutex_lock(&k_lock);
    p = k_buffer;
    if (p) {
      // 写文件
      auto fp = Core::Instance()->file();
      fprintf(fp, "%s %-8s %s\n", p->date, p->file_level_buf,
              p->buffer.c_str());
      fflush(fp);
      // 释放内存
      delete p;
      k_buffer = nullptr;
    }
    pthread_mutex_unlock(&k_lock);
    sem_post(&WriterProducer::m_k_sem);
  }
}

}  // namespace logger