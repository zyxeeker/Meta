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

#if 0
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
  using ptr = std::shared_ptr<LogEvent>;
  LogEvent(const char *file = nullptr, const char *caller = nullptr,
           uint32_t line = 0, uint32_t thread_id = 0, uint64_t time = 0,
           std::string thread_name = "main", std::string msg = "") :
      m_file_name(file), m_caller(caller), m_thread_id(thread_id),
      m_time(time), m_thread_name(thread_name), m_msg(msg) {};

  const char *file_name() { return m_file_name; }
  const char *caller() { return m_caller; }
  uint32_t line() const { return m_line; }
  uint32_t thread_id() const { return m_thread_id; }
  uint64_t time() const { return m_time; }
  const std::string &thread_name() { return m_thread_name; }
  const std::string &msg() { return m_msg; }

 private:
  const char *m_file_name;        // 文件名
  const char *m_caller;           // 函数名
  uint32_t m_line;                // 行号
  uint32_t m_thread_id;           // 线程id
  uint64_t m_time;                // 时间戳
  std::string m_thread_name;      // 线程名
  std::string m_msg;              // 内容
};

class LogOutput;

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
  LogLevel::level level() { return m_level; }
 private:
  void Logger(LogLevel::level level, LogEvent::ptr event);
  void Append(std::shared_ptr<LogOutput> out);
  void Remove();

 private:
  std::string m_name;
  LogLevel::level m_level;
  std::list<std::shared_ptr<LogOutput>> m_output_queue;
};

// 日志格式解析器
class LogFormatter {
 public:
  typedef std::shared_ptr<LogFormatter> ptr;

  void Init(std::string pattern);

  // 日志参数模块
  class LogFormatterParam {
   public:
    typedef std::shared_ptr<LogFormatter::LogFormatterParam> ptr;
    virtual ~LogFormatterParam() {};
    virtual void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) = 0;
  };
  const std::list<LogFormatterParam::ptr> &pattern() { return m_pattern; }

 private:
  std::list<LogFormatterParam::ptr> m_pattern;
};

// 日志输出
class LogOutput {
 public:
  LogOutput(LogFormatter::ptr formatter) : m_formatter(formatter) {};
  virtual ~LogOutput() {};

  virtual void Print(Log::ptr log, LogEvent::ptr event) = 0;

 protected:
  LogFormatter::ptr m_formatter;
};

// 控制台输出
class LogConsoleOutput : public LogOutput {
 public:
  typedef std::shared_ptr<LogConsoleOutput> ptr;
  LogConsoleOutput(LogFormatter::ptr formatter) : LogOutput(formatter) {}

  void Print(Log::ptr log, LogEvent::ptr event) override;
};

// 文件输出
class LogFileOutput : public LogOutput {
 public:
  typedef std::shared_ptr<LogFileOutput> ptr;
  LogFileOutput(std::string name, LogFormatter::ptr formatter) :
      LogOutput(formatter), m_name(name) {};
  LogFileOutput(std::string path, std::string name, LogFormatter::ptr formatter) :
      LogOutput(formatter), m_path(path), m_name(name) {};

  void Init();
  void Print(Log::ptr log, LogEvent::ptr event) override;

 private:
  bool OpenFile();

 private:
  std::ofstream m_stream;
  std::string m_path;
  std::string m_name;
};

} // namespace meta
#endif
#endif  // SRC_LOGGER_LOG_H_