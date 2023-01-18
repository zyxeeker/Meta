#ifndef SRC_LOGGER_LOG_H_
#define SRC_LOGGER_LOG_H_

#include <string>
#include <memory>
#include <fstream>
#include <list>

namespace logger {

// 颜色
const char k_black[] = "\033[30m";
const char k_red[] = "\033[31m";
const char k_green[] = "\033[32m";
const char k_yellow[] = "\033[33m";
const char k_blue[] = "\033[34m";
const char k_magenta[] = "\033[35m";
const char k_cyan[] = "\033[36m";
const char k_white[] = "\033[37m";
// 结束符
const char k_color_end[] = "\033[0m";

// 等级
enum LogLevel { DEBUG = 0, INFO, WARN, ERROR };

// buffer
struct LogBuffer {
  char date[50];
  char file_level_buf[10];
  char level_buf[20];
  std::string buffer;
};

}  // namespace logger

namespace meta {

// 日志等级
class LogLevel {
 public:
  enum level {
    UNKNOWN = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
  };

  static const char *ToString(LogLevel::level level);
};

// 日志事件
class LogEvent {
 public:
  typedef std::shared_ptr<LogEvent> ptr;

  const char *file_name() { return m_file_name; }
  const char *caller() { return m_caller; }
  uint32_t line() const { return m_line; }
  uint32_t thread_id() const { return m_thread_id; }
  uint64_t time() const { return m_time; }
  const std::string &thread_name() { return m_thread_name; }
  const std::string &msg() { return m_msg; }

 private:
  const char *m_file_name;      // 文件名
  const char *m_caller;         // 函数名
  uint32_t m_line = 0;          // 行号
  uint32_t m_thread_id = 0;     // 线程id
  uint64_t m_time = 0;          // 时间戳
  std::string m_thread_name;    // 线程名
  std::string m_msg;            // 内容
};

// 日志器
class Log {
 public:
  typedef std::shared_ptr<Log> ptr;

  Log(LogLevel::level level = LogLevel::INFO, std::string name = "root") :
      m_level(level), m_name(name) {};

  void Debug(LogEvent::ptr event);
  void Info(LogEvent::ptr event);
  void Warn(LogEvent::ptr event);
  void Error(LogEvent::ptr event);
  void Fatal(LogEvent::ptr event);

  std::string name() { return m_name; };
 private:
  void Logger(LogLevel::level level, LogEvent::ptr event);

 private:
  std::string m_name;
  LogLevel::level m_level;
};

// 日志格式化
class LogFormatter {
 public:
  void Init(std::string pattern);

  // 格式化子模块
  class LogFormatterItem {
   public:
    typedef std::shared_ptr<LogFormatter::LogFormatterItem> ptr;

//    virtual ~LogFormatterItem();
    virtual void Format(std::ostream os, Log::ptr log, LogEvent::ptr event) = 0;
  };
 private:
  std::list<LogFormatterItem::ptr> m_formatters;

};

// 日志输出
class LogOutput {
 public:
  ~LogOutput();
  virtual void Handle(Log::ptr log, LogEvent::ptr event) = 0;
};

class LogConsoleOutput : public LogOutput {
 public:
  void Handle(Log::ptr log, LogEvent::ptr event) override;
};

class LogFileOutput : public LogOutput {
 public:
  LogFileOutput(std::string file_name) : m_file_name(file_name) {};
  void Handle(Log::ptr log, LogEvent::ptr event) override;

 private:
  void Write(std::ofstream os, std::string buf);
 private:
  std::ofstream m_file_stream;
  std::string m_file_name;
};

} // namespace meta

#endif  // SRC_LOGGER_LOG_H_