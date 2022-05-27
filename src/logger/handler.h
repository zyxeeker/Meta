#ifndef SRC_LOGGER_HANDLER_H_
#define SRC_LOGGER_HANDLER_H_

#include <memory>

#include "log.h"
#include "thread/pool.h"

namespace logger {

class Core;

class Handler {
 public:
  Handler(LogLevel _level) {
    TransLevel(_level);
    GetTime();
  }
  ~Handler();

  Handler& operator<<(const char* v) {
    m_buffer.buffer.append(v);
    return *this;
  }
  Handler& operator<<(int v) {
    m_buffer.buffer.append(std::to_string(v));
    return *this;
  }

 private:
  // 获取时间
  void GetTime();
  // 转换等级
  void TransLevel(LogLevel level);
  // 写入等级到文件和控制台缓冲区
  inline void WriteToBuffer(const char* level, const char* color);
  // 写入控制台
  inline void WriteToConsole();

 private:
  LogBuffer m_buffer;
};

}  // namespace logger

#endif  // SRC_LOGGER_HANDLER_H_
