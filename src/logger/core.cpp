#include "core.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>

namespace logger {

Core* Core::inst = nullptr;

Core::Core() {
  m_pool.reset(new thread::Pool(50, 1));

  char m_date[50]{0};
  time_t now = time(nullptr);
  strftime(m_date, 50, "%Y_%m_%d_%H_%M", localtime(&now));

  char file_name[60]{0};
  sprintf(file_name, "log_%s.log", m_date);
  // log_2022_05_27_10_48.txt
  m_file.open(file_name, std::ios::app);
}

Handler Core::Out(LogLevel level) { return Handler(level); }

void Core::AppendTask(LogBuffer buf) {
  // TODO: 修改判断条件
  if (m_cur_writer_num >= 50) m_cur_writer_num = 0;

  auto p = m_writer + m_cur_writer_num;
  p->Init(buf, &m_file);

  if (m_pool->Append(p) != thread::Pool::ADD_REQUEST_SUCCESS)
    MERROR() << "Logger queue is full!";

  ++m_cur_writer_num;
}

}  // namespace logger