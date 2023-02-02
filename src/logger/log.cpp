/**
 * @author: zyxeeker
 * @createTime: 2023/01/12 8:39 AM
 * @description: 日志相关实现
 */

#include "log.h"
#include <unistd.h>
#include <ctime>
#include <map>
#include <stack>
#include <functional>
#include <iostream>

#define DEFAULT_DATETIME_PATTERN  "%Y-%m-%d %H:%M"
#define EMPTY_PARAM               ""

namespace meta {

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

//// 日志参数模块 Begin
// 日志名
class LName : public LogFormatter::LogFormatterParam {
 public:
  LName(const std::string &buf) {}
  void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) override {
    os << log->name();
  }
};

// 日志等级
class LLevel : public LogFormatter::LogFormatterParam {
 public:
  LLevel(const std::string &buf) {}
  void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) override {
    os << LogLevel::ToString(log->level());
  }
};

// 日期
class LDateTime : public LogFormatter::LogFormatterParam {
 public:
  LDateTime(const std::string fmt) : m_fmt(fmt) {}
  void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) override {
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
  void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) override {
    os << m_buf;
  }
 private:
  const std::string m_buf;
};

// Tab
class LTab : public LogFormatter::LogFormatterParam {
 public:
  LTab(const std::string &buf) {}
  void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) override {
    os << "\t";
  }
};

// 换行
class LNewLine : public LogFormatter::LogFormatterParam {
 public:
  LNewLine(const std::string &buf) {}
  void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) override {
    os << "\n";
  }
};

// 线程名
class LThreadName : public LogFormatter::LogFormatterParam {
 public:
  LThreadName(const std::string &buf) {}
  void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) override {
    os << event->thread_name();
  }
};

// 线程ID
class LThreadId : public LogFormatter::LogFormatterParam {
 public:
  LThreadId(const std::string &buf) {}
  void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) override {
    os << std::to_string(event->thread_id());
  }
};

// 文件名
class LFileName : public LogFormatter::LogFormatterParam {
 public:
  LFileName(const std::string &buf) {}
  void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) override {
    auto f = event->file_name();
    f ? os << f : os << EMPTY_PARAM;
  }
};

// 函数名
class LCaller : public LogFormatter::LogFormatterParam {
 public:
  LCaller(const std::string &buf) {}
  void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) override {
    auto c = event->caller();
    c ? os << c : os << EMPTY_PARAM;
  }
};

// 行号
class LLineNum : public LogFormatter::LogFormatterParam {
 public:
  LLineNum(const std::string &buf) {}
  void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) override {
    os << std::to_string(event->line());
  }
};

// 消息
class LMsg : public LogFormatter::LogFormatterParam {
 public:
  LMsg(const std::string &buf) {}
  void Format(std::ostream &os, Log::ptr log, LogEvent::ptr event) override {
    os << event->msg();
  }
};
//// 日志参数模块 End

void LogFormatter::Init(std::string pattern) {
  // 日志格式回调函数Map
  static std::map<std::string, std::function<LogFormatterParam::ptr(const std::string &str)>> k_formatter_param_cb = {
#define XX(str, C) \
    {#str, [](const std::string& buf){ return LogFormatterParam::ptr(new C(buf)); }}, \

      XX(r, LName)             // r 日志名称
      XX(d, LDateTime)         // d 时间
      XX(P, LLevel)            // P 日志等级
      XX(S, LString)           // S 原始字符串
      XX(t, LTab)              // t \t
      XX(n, LNewLine)          // n \n
      XX(T, LThreadId)         // T Thread ID
      XX(N, LThreadName)       // N Thread Name
      XX(F, LFileName)         // F 文件名
      XX(C, LCaller)           // C 函数名
      XX(L, LLineNum)          // L 行号
      XX(m, LMsg)              // m 消息
#undef XX
  };

  std::string pattern_sub_str;
  for (uint32_t i = 0, j; i < pattern.size(); i++) {
    // 不为%
    if (pattern[i] != '%') {
      pattern_sub_str.append(1, pattern[i]);
      continue;
    }
    // 为%
    m_pattern.push_back(k_formatter_param_cb["S"](pattern_sub_str));
    pattern_sub_str.clear();

    j = ++i;
    if (j < pattern.size()) {
      // DateTime fmt Begin
      if (pattern[j] == 'd') {
        ++j;
        if (std::isspace(pattern[j]) || pattern[j] != '{') {
          m_pattern.push_back(k_formatter_param_cb["d"](DEFAULT_DATETIME_PATTERN));
          continue;
        }

        uint32_t front_bracket_index = j, front_bracket_count = 0, last_bracket_index = 0;
        for (++j; j < pattern.size(); j++) {
          if (pattern[j] == '{')
            ++front_bracket_count;
          if (pattern[j] == '}') {
            last_bracket_index = j;
            if (front_bracket_count > 0)
              --front_bracket_count;
            else
              break;
          }
        }
        if (last_bracket_index <= front_bracket_index) {
          std::cout << "Illegal pattern for datetime" << last_bracket_index << " " << front_bracket_index << std::endl;
          break;
        }
        std::string date_sub_string = pattern.substr(++front_bracket_index,
                                                     last_bracket_index - front_bracket_index - 1);
        std::cout << date_sub_string << std::endl;

        m_pattern.push_back(k_formatter_param_cb["d"](date_sub_string));
        i = last_bracket_index;
        // DateTime fmt End
      } else {
        auto res = k_formatter_param_cb.find(pattern.substr(j, 1));
        if (res == k_formatter_param_cb.end()) {
          std::cout << "Illegal pattern: " << pattern[j] << j << std::endl;
          break;
        }
        m_pattern.push_back(res->second(EMPTY_PARAM));
      }
    }
  }
}

void LogConsoleOutput::Print(Log::ptr log, LogEvent::ptr event) {
  for (auto &i : m_formatter->pattern()) {
    i->Format(std::cout, log, event);
  }
  std::cout << std::endl;
}

void LogFileOutput::Init() {
  OpenFile();
}

void LogFileOutput::Print(Log::ptr log, LogEvent::ptr event) {
  for (auto &i : m_formatter->pattern()) {
    i->Format(m_stream, log, event);
  }
  m_stream << std::endl;
}

bool LogFileOutput::OpenFile() {
  if (m_path.empty()) {
    auto path = get_current_dir_name();
    m_path = path;
  }
  std::string f;
  f.append(m_path);
  f.append("/");
  f.append(m_name);
  m_stream.open(f.c_str(), std::ios::out | std::ios::app);
  if (!m_stream.is_open()) {
    std::cout << "Failed to open:" << f << std::endl;
    return false;
  }
  return true;
}

} // namespace meta