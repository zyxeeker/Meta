#include "core.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>

namespace logger {

Core* Core::inst = nullptr;

Core::Core() {
  m_writer_producer_pool = new thread::Pool(1, WriterProducer::Process);
  m_writer_consumer_pool = new thread::Pool(1, WriterConsumer::Process);

  char m_date[50]{0};
  time_t now = time(nullptr);
  strftime(m_date, 50, "%Y_%m_%d_%H_%M", localtime(&now));

  char file_name[60]{0};
  sprintf(file_name, "log_%s.log", m_date);
  // log_2022_05_27_10_48.txt
  m_file = fopen(file_name, "ab");

  WriterProducer::Start();
}

Handler Core::Out(LogLevel level) { return Handler(level); }

void Core::AppendTask(LogBuffer* buf) {
  pthread_mutex_lock(&m_lock);
  m_list.push(buf);
  pthread_mutex_unlock(&m_lock);
}

void* Core::GetObj() {
  void* p = nullptr;
  pthread_mutex_lock(&m_lock);
  if (m_list.size() != 0) {
    p = m_list.front();
    m_list.pop();
  }
  pthread_mutex_unlock(&m_lock);
  return p;
}

}  // namespace logger