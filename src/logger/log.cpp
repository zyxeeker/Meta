/**
 * @author: zyxeeker
 * @createTime: 2023/01/12 8:39 AM
 * @description: ${description}
 */

#include "log.h"
#include <map>
#include <stack>
#include <functional>
#include <iostream>

namespace meta {

enum LogBlock {
  STR_BLOCK     = 0,
  PATTERN_BLOCK = 1,
  BLOCK_NUM     = 2
};

// 最低输出的日志级别
static LogLevel::level k_level = LogLevel::INFO;

const char *LogLevel::ToString(LogLevel::level level) {
  switch (level) {
#define TRANS(name) \
  case name:\
      return #name;

    TRANS(DEBUG)
    TRANS(INFO)
    TRANS(WARN)
    TRANS(ERROR)
    TRANS(FATAL)
#undef TRANS
    default:
      return "UNKNOWN";
  }
}

void Log::Debug(LogEvent::ptr event) {
  Logger(LogLevel::DEBUG, event);
}

void Log::Info(LogEvent::ptr event) {
  Logger(LogLevel::INFO, event);
}

void Log::Error(LogEvent::ptr event) {
  Logger(LogLevel::ERROR, event);
}

void Log::Fatal(LogEvent::ptr event) {
  Logger(LogLevel::FATAL, event);
}

void Log::Logger(LogLevel::level level, LogEvent::ptr event) {
  // TODO:
}

// 格式化模块
class DateTimeFormat : public LogFormatter::LogFormatterItem {
 public:
  // TODO:
  DateTimeFormat(const std::string& fmt) : m_fmt(fmt) {}
  void Format(std::ostream os, Log::ptr log, LogEvent::ptr event) override {
    // TODO:
  }
 private:
  const std::string& m_fmt;
};

class StringBlockFormat : public LogFormatter::LogFormatterItem{
 public:
  StringBlockFormat(const std::string& buf) : m_buf(buf){}
  void Format(std::ostream os, Log::ptr log, LogEvent::ptr event) override {
    os << m_buf;
  }
 private:
  const std::string& m_buf;
};

class NameFormat : public LogFormatter::LogFormatterItem {
 public:
  NameFormat(const std::string& buf) {}
  void Format(std::ostream os, Log::ptr log, LogEvent::ptr event) override {
    os << log->name();
  }
};

class TabFormat : public LogFormatter::LogFormatterItem {
 public:
  TabFormat(const std::string& buf) {}
  void Format(std::ostream os, Log::ptr log, LogEvent::ptr event) override {
    os << "\t";
  }
};

class NewLineFormat : public LogFormatter::LogFormatterItem {
 public:
  NewLineFormat(const std::string& buf) {}
  void Format(std::ostream os, Log::ptr log, LogEvent::ptr event) override {
    os << "\n";
  }
};

class ThreadNameFormat : public LogFormatter::LogFormatterItem {
 public:
  ThreadNameFormat(const std::string& buf) {}
  void Format(std::ostream os, Log::ptr log, LogEvent::ptr event) override {
    os << event->thread_name();
  }
};

class ThreadIdFormat : public LogFormatter::LogFormatterItem {
 public:
  ThreadIdFormat(const std::string& buf) {}
  void Format(std::ostream os, Log::ptr log, LogEvent::ptr event) override {
    os << std::to_string(event->thread_id());
  }
};

class FileNameFormat : public LogFormatter::LogFormatterItem {
 public:
  FileNameFormat(const std::string& buf) {}
  void Format(std::ostream os, Log::ptr log, LogEvent::ptr event) override {
    os <<event->file_name();
  }
};

class LineFormat : public LogFormatter::LogFormatterItem {
 public:
  LineFormat(const std::string& buf) {}
  void Format(std::ostream os, Log::ptr log, LogEvent::ptr event) override {
    os << std::to_string(event->line());
  }
};

class MsgFormat : public LogFormatter::LogFormatterItem {
 public:
  MsgFormat(const std::string& buf) {}
  void Format(std::ostream os, Log::ptr log, LogEvent::ptr event) override {
    os << event->msg();
  }
};

void LogFormatter::Init(std::string pattern) {
  // 格式化回调函数Map
  static std::map<std::string, std::function<LogFormatterItem::ptr(const std::string &str)>> k_formatter_item_cb = {
#define XX(str, C) \
    {#str, [](const std::string& buf){ return LogFormatterItem::ptr(new C(buf)); }}, \

      XX(r, NameFormat)             // r 日志名称
      XX(d, DateTimeFormat)         // d 日志时间
      XX(t, TabFormat)              // t \t
      XX(n, NewLineFormat)          // n \n
      XX(T, ThreadIdFormat)         // T Thread ID
      XX(N, ThreadNameFormat)       // N Thread Name
      XX(F, FileNameFormat)         // F 文件名
      XX(L, LineFormat)             // L 行号
      XX(m, MsgFormat)              // m 消息
#undef XX
  };

#if 0
  pattern = "%d{%Y %M %D %H %M %S} %p %F %L %m";
#endif
  std::string pattern_sub_str;
  for (int i = 0; i < pattern.size(); i++) {
    // 不为%
    if (pattern[i] != '%') {
      pattern_sub_str.append(1, pattern[i]);
      continue;
    }
    // 为%
    // TODO:Append String format item
    pattern_sub_str.clear();

    if (++i < pattern.size()) {
      auto res = k_formatter_item_cb.find(pattern.substr(i,1));
      if (res == k_formatter_item_cb.end()) {
        std::cout << "Unknown pattern: " << pattern[i] << std::endl;
        break;
      }
      // TODO:Append formatter

      // Datetime Fmt Begin
      if (pattern[i++] == 'd') {
        if (std::isspace(pattern[i]) || pattern[i] != '{') {
          continue;
        }
        uint32_t front_bracket_index = i, last_bracket_index = 0;
        std::stack<uint32_t> front_bracket;
        ++i;
        while (i < pattern.size()) {
          if (pattern[i] == '{') {
            front_bracket.push(i);
          }
          if (pattern[i] == '}') {
            if (!front_bracket.empty()) {
              front_bracket.pop();
              last_bracket_index = i;
            } else {
              last_bracket_index = i;
              break;
            }
          }
          ++i;
        }
        if (last_bracket_index <= front_bracket_index) {
          std::cout << "Illegal pattern for datetime" << last_bracket_index <<" " << front_bracket_index << std::endl;
          break;
        }
        ++front_bracket_index;
        std::string date_sub_string = pattern.substr(front_bracket_index, last_bracket_index - front_bracket_index);
        std::cout <<date_sub_string<<std::endl;
        // TODO:append date format
        continue;
      }
      // Datetime Fmt End
    }
 }
}

void LogFileOutput::Write(std::ofstream os, std::string buf) {
  os << buf;
}

} // namespace meta