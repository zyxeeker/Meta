#ifndef SRC_LOGGER_CORE_H_
#define SRC_LOGGER_CORE_H_

#include <cstdio>
#include <memory>
#include <queue>
#include <string>

#include "handler.h"
#include "log.h"
#include "thread/pool.h"
#include "writer.h"

#define LOGGER_DETAIL                                              \
  "[" << __FUNCTION__ << "(" << __FILE__ << ":" << __LINE__ << ")" \
      << "]:"

#define MINFO() \
  logger::Core::Instance()->Out(logger::LogLevel::INFO) << LOGGER_DETAIL
#define MDEBUG() \
  logger::Core::Instance()->Out(logger::LogLevel::DEBUG) << LOGGER_DETAIL
#define MWARN() \
  logger::Core::Instance()->Out(logger::LogLevel::WARN) << LOGGER_DETAIL
#define MERROR() \
  logger::Core::Instance()->Out(logger::LogLevel::ERROR) << LOGGER_DETAIL

namespace logger {

class Core {
 public:
  static Core* Instance() {
    if (!inst) inst = new Core();
    return inst;
  }
  Core();
  ~Core() { fclose(m_file); }

  void AppendTask(LogBuffer* buf);

  void* GetObj();

  FILE* file() { return m_file; }

  Handler Out(LogLevel level);

 private:
  static Core* inst;

  pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;
  // 文件
  FILE* m_file = nullptr;
  // 当前可用写者序号
  int m_cur_writer_num = 0;
  // 缓冲
  std::queue<LogBuffer*> m_list;
  // 生产者和消费者
  thread::Pool* m_writer_producer_pool = nullptr;
  thread::Pool* m_writer_consumer_pool = nullptr;
};

}  // namespace logger

#endif  // SRC_LOGGER_CORE_H_
