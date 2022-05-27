#ifndef SRC_LOGGER_CORE_H_
#define SRC_LOGGER_CORE_H_

#include <fstream>
#include <memory>
#include <string>

#include "handler.h"
#include "log.h"
#include "thread/pool.h"
#include "writer.h"

#define MINFO() logger::Core::Instance()->Out(logger::LogLevel::INFO)
#define MDEBUG() logger::Core::Instance()->Out(logger::LogLevel::DEBUG)
#define MWARN() logger::Core::Instance()->Out(logger::LogLevel::WARN)
#define MERROR() logger::Core::Instance()->Out(logger::LogLevel::ERROR)

namespace logger {

class Core {
 public:
  static Core* Instance() {
    if (!inst) inst = new Core();
    return inst;
  }
  Core();
  ~Core() { m_file.close(); }

  void AppendTask(LogBuffer buf);

  void Stop() { m_pool->Stop(); }

  Handler Out(LogLevel level);

 private:
  static Core* inst;
  // 异步写文件池
  std::shared_ptr<thread::Pool> m_pool;
  // 文件
  std::ofstream m_file;
  // 当前可用写者序号
  int m_cur_writer_num = 0;
  // 写者
  Writer m_writer[50];
};

}  // namespace logger

#endif  // SRC_LOGGER_CORE_H_
