/**
 * @author: zyxeeker
 * @createTime: 2023/02/06 8:51 AM
 * @description: 日志
 */

#ifndef META_INCLUDE_LOG_H_
#define META_INCLUDE_LOG_H_

#include <unistd.h>
#include <memory>
#include <sstream>
#include <list>
#include <functional>
#include "util.h"

#define FORMATTER_APPEND_PARAM(K, C) \
meta::LogFormatter::Append(#K, [](const std::string &buf){ return meta::LogFormatter::LogFormatterParam::ptr(new C(buf))});
#define FORMATTER_REMOVE_PARAM(K) \
meta::LogFormatter::Remove(#K);

#define META_EVENT std::make_shared<meta::LogEvent>(__FILE__, __FUNCTION__, __LINE__, 0, 0)

#define META_DEBUG()  \
  meta::LogWrap(meta::LoggerMgr::Instance().GetLogger(meta::LogLevel::DEBUG), META_EVENT).event()->stream()
#define META_INFO()   \
  meta::LogWrap(meta::LoggerMgr::Instance().GetLogger(meta::LogLevel::INFO), META_EVENT).event()->stream()
#define META_WARN()   \
  meta::LogWrap(meta::LoggerMgr::Instance().GetLogger(meta::LogLevel::WARN), META_EVENT).event()->stream()
#define META_ERROR()  \
  meta::LogWrap(meta::LoggerMgr::Instance().GetLogger(meta::LogLevel::ERROR), META_EVENT).event()->stream()
#define META_FATAL()  \
  meta::LogWrap(meta::LoggerMgr::Instance().GetLogger(meta::LogLevel::FATAL), META_EVENT).event()->stream()

#define META_DEBUG_FMT(FMT, ...) \
  meta::LogWrap(meta::LoggerMgr::Instance().GetLogger(meta::LogLevel::DEBUG), META_EVENT).event()->Printf(FMT, __VA_ARGS__)
#define META_INFO_FMT(FMT, ...) \
  meta::LogWrap(meta::LoggerMgr::Instance().GetLogger(meta::LogLevel::INFO), META_EVENT).event()->Printf(FMT, __VA_ARGS__)
#define META_WARN_FMT(FMT, ...) \
  meta::LogWrap(meta::LoggerMgr::Instance().GetLogger(meta::LogLevel::WARN), META_EVENT).event()->Printf(FMT, __VA_ARGS__)
#define META_ERROR_FMT(FMT, ...) \
  meta::LogWrap(meta::LoggerMgr::Instance().GetLogger(meta::LogLevel::ERROR), META_EVENT).event()->Printf(FMT, __VA_ARGS__)
#define META_FATAL_FMT(FMT, ...) \
  meta::LogWrap(meta::LoggerMgr::Instance().GetLogger(meta::LogLevel::FATAL), META_EVENT).event()->Printf(FMT, __VA_ARGS__)

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
           std::string thread_name = "main");

  void Printf(const char* fmt, ...);
  inline const char *file_name() { return _file_name; }
  inline const char *caller() { return _caller; }
  inline uint32_t line() const { return _line; }
  inline uint32_t thread_id() const { return _thread_id; }
  inline uint64_t time() const { return _time; }
  inline const std::string &thread_name() { return _thread_name; }
  inline std::stringstream &stream() { return _stream; }
private:
  const char *_file_name;                 // 文件名
  const char *_caller;                    // 函数名
  const uint32_t _line;                   // 行号
  const uint32_t _thread_id;              // 线程id
  const uint64_t _time;                   // 时间戳
  const std::string _thread_name;         // 线程名
  std::stringstream _stream;
};

class LogOutput;
class LogFormatter;
// 日志器
class Logger : public std::enable_shared_from_this<Logger>{
 public:
  using ptr = std::shared_ptr<Logger>;
  Logger(LogLevel::level level = LogLevel::INFO, std::string name = "root");
  void Print(const LogEvent::ptr& event);
  void AppendOutput(std::shared_ptr<LogOutput> out);
  void RemoveOutput(std::shared_ptr<LogOutput> out);
  const std::string& name() const { return _name; };
  LogLevel::level level() const { return _level; }
 private:
  std::list<std::shared_ptr<LogOutput>> _output_list;
  std::string _name;
  LogLevel::level _level;
};

// 日志格式解析器
class LogFormatter {
 public:
  using ptr = std::shared_ptr<LogFormatter>;
  // 日志参数模块
  class LogFormatterParam {
   public:
    using ptr = std::shared_ptr<LogFormatter::LogFormatterParam>;
    virtual ~LogFormatterParam() {};
    virtual inline void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) = 0;
  };
  void Init(std::string pattern);
  static void Append(const char* key, std::function<LogFormatterParam::ptr(const std::string&)>);
  static void Remove(const std::string& key);
  const std::list<LogFormatterParam::ptr> &pattern() { return _pattern; }
 private:
  std::list<LogFormatterParam::ptr> _pattern;
};

// 日志输出
class LogOutput {
 public:
  LogOutput(LogFormatter::ptr formatter) : _formatter(formatter) {};
  virtual ~LogOutput() {};
  virtual void Print(Logger::ptr log, LogEvent::ptr event) = 0;
 protected:
  LogFormatter::ptr _formatter;
};

// 日志装饰器
class LogWrap {
 public:
  LogWrap(const Logger::ptr &log, LogEvent::ptr event);
  ~LogWrap();
  LogEvent::ptr event() { return _event; }
 private:
  const Logger::ptr _logger;
  LogEvent::ptr _event;
};

// 日志管理器
class LoggerManager {
 public:
  LoggerManager();
  Logger::ptr GetLogger(LogLevel::level level);
  void Append(LogLevel::level level, Logger::ptr log);
  void Remove(LogLevel::level level);
 private:
  Logger::ptr _default_logger;
};

using LoggerMgr = Single<LoggerManager>;

} // namespace meta

#endif //META_INCLUDE_LOG_H_
