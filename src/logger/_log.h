/**
 * @author: zyxeeker
 * @createTime: 2023/02/06 10:39 AM
 * @description: log实现
 */

#ifndef META_SRC_LOGGER__LOG_H_
#define META_SRC_LOGGER__LOG_H_

#include "../include/log.h"
#include <fstream>

#define DEFAULT_DATETIME_PATTERN      "%Y-%m-%d %H:%M"
#define EMPTY_PARAM                   ""
#define DEFAULT_FORMATTER_PATTERN     "%d {%r}(%F:%L)[%P]|%N| %m"

namespace meta {

//// 日志参数模块 Begin
// 日志名
class LName : public LogFormatter::LogFormatterParam {
 public:
  LName(const std::string &buf) {}
  void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) override {
    os << log->name();
  }
};

// 日志等级
class LLevel : public LogFormatter::LogFormatterParam {
 public:
  LLevel(const std::string &buf) {}
  void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) override {
    os << LogLevel::ToString(log->level());
  }
};

// 日期
class LDateTime : public LogFormatter::LogFormatterParam {
 public:
  LDateTime(const std::string fmt) : m_fmt(fmt) {}
  void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) override {
    char date[50]{0};
    time_t now = time(nullptr);
    strftime(date, 50, m_fmt.c_str(), localtime(&now));
    os << date;
  }
 private:
  const std::string m_fmt;
};

// 原始字符
class LString : public LogFormatter::LogFormatterParam {
 public:
  LString(const std::string buf) : m_buf(buf) {}
  void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) override {
    os << m_buf;
  }
 private:
  const std::string m_buf;
};

// Tab
class LTab : public LogFormatter::LogFormatterParam {
 public:
  LTab(const std::string &buf) {}
  void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) override {
    os << "\t";
  }
};

// 换行
class LNewLine : public LogFormatter::LogFormatterParam {
 public:
  LNewLine(const std::string &buf) {}
  void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) override {
    os << "\n";
  }
};

// 线程名
class LThreadName : public LogFormatter::LogFormatterParam {
 public:
  LThreadName(const std::string &buf) {}
  void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) override {
    os << event->thread_name();
  }
};

// 线程ID
class LThreadId : public LogFormatter::LogFormatterParam {
 public:
  LThreadId(const std::string &buf) {}
  void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) override {
    os << std::to_string(event->thread_id());
  }
};

// 文件名
class LFileName : public LogFormatter::LogFormatterParam {
 public:
  LFileName(const std::string &buf) {}
  void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) override {
    auto f = event->file_name();
    f ? os << f : os << EMPTY_PARAM;
  }
};

// 函数名
class LCaller : public LogFormatter::LogFormatterParam {
 public:
  LCaller(const std::string &buf) {}
  void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) override {
    auto c = event->caller();
    c ? os << c : os << EMPTY_PARAM;
  }
};

// 行号
class LLineNum : public LogFormatter::LogFormatterParam {
 public:
  LLineNum(const std::string &buf) {}
  void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) override {
    os << std::to_string(event->line());
  }
};

// 消息
class LMsg : public LogFormatter::LogFormatterParam {
 public:
  LMsg(const std::string &buf) {}
  void Format(std::ostream &os, Logger::ptr log, LogEvent::ptr event) override {
    os << event->stream().str();
  }
};
//// 日志参数模块 End

// 控制台输出
class LogConsoleOutput : public LogOutput {
 public:
  using ptr = std::shared_ptr<LogConsoleOutput>;
  LogConsoleOutput(LogFormatter::ptr formatter) : LogOutput(formatter) {}
  void Print(Logger::ptr log, LogEvent::ptr event) override;
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
  void Print(Logger::ptr log, LogEvent::ptr event) override;
 private:
  void OpenFile();
  std::ofstream m_stream;
  std::string m_path;
  std::string m_name;
};

} // namespace meta

#endif //META_SRC_LOGGER__LOG_H_
