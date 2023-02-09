/**
 * @author: zyxeeker
 * @createTime: 2023/02/06 10:43 AM
 * @description: log实现
 */

#include "_log.h"
#include <map>
#include <functional>
#include <cstdarg>
#include <algorithm>
#include <iostream>

namespace meta {

using formatter_param_cb = std::map<std::string, std::function<LogFormatter::LogFormatterParam::ptr(const std::string &str)>>;
static formatter_param_cb & formatter_param_cb_map(){
// 日志格式回调函数Map
  static std::map<std::string, std::function<LogFormatter::LogFormatterParam::ptr(const std::string &str)>> k_formatter_param_cb = {
#define XX(str, C) \
    {#str, [](const std::string& buf){ return LogFormatter::LogFormatterParam::ptr(new C(buf)); }}

      XX(r, LName),             // r 日志名称
      XX(d, LDateTime),         // d 时间
      XX(P, LLevel),            // P 日志等级
      XX(S, LString),           // S 原始字符串
      XX(t, LTab),              // t \t
      XX(n, LNewLine),          // n \n
      XX(T, LThreadId),         // T Thread ID
      XX(N, LThreadName),       // N Thread Name
      XX(F, LFileName),         // F 文件名
      XX(C, LCaller),           // C 函数名
      XX(L, LLineNum),          // L 行号
      XX(m, LMsg),              // m 消息
#undef XX
  };
  return k_formatter_param_cb;
}

//
static std::map<LogLevel::level, Logger::ptr> k_logger_mgr;

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
LogEvent::LogEvent(const char *file, const char *caller, uint32_t line,
                   uint32_t thread_id, uint64_t time, std::string thread_name) :
                   _file_name(file), _caller(caller), _thread_id(thread_id),
                   _time(time), _thread_name(thread_name), _line(line) {};

void LogEvent::Printf(const char *fmt, ...) {
  char* buf = nullptr;
  va_list args;
  va_start(args, fmt);
  uint32_t res = vasprintf(&buf, fmt, args);
  if (res != -1) {
    _stream << buf;
    free(buf);
  }
}

Logger::Logger(LogLevel::level level, std::string name) : _level(level), _name(name) {}

void Logger::Print(const LogEvent::ptr& event) {
  for (auto& i : _output_list)
    i->Print(shared_from_this(), event);
}

void Logger::AppendOutput(std::shared_ptr<LogOutput> out) {
  _output_list.push_back(out);
}

void Logger::RemoveOutput(std::shared_ptr<LogOutput> out) {
  auto res = std::find(_output_list.begin(), _output_list.end(), out);
  if (res != _output_list.end())
    _output_list.erase(res);
}

void LogFormatter::Init(std::string pattern) {
  std::string pattern_sub_str;
  for (uint32_t i = 0, j; i < pattern.size(); i++) {
    // 不为%
    if (pattern[i] != '%') {
      pattern_sub_str.append(1, pattern[i]);
      continue;
    }
#if 1
    // 为%
    _pattern.push_back(formatter_param_cb_map()["S"](pattern_sub_str));
    pattern_sub_str.clear();

    j = ++i;
    if (j < pattern.size()) {
      // DateTime fmt Begin
      if (pattern[j] == 'd') {
        ++j;
        if (std::isspace(pattern[j]) || pattern[j] != '{') {
          _pattern.push_back(formatter_param_cb_map()["d"](DEFAULT_DATETIME_PATTERN));
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
          META_ERROR_FMT("Illegal pattern for datetime at pattern: %d %d", last_bracket_index, front_bracket_index);
          break;
        }
        std::string date_sub_string = pattern.substr(++front_bracket_index,
                                                     last_bracket_index - front_bracket_index - 1);
        std::cout << date_sub_string << std::endl;

        _pattern.push_back(formatter_param_cb_map()["d"](date_sub_string));
        i = last_bracket_index;
        // DateTime fmt End
      } else {
        auto res = formatter_param_cb_map().find(pattern.substr(j, 1));
        if (res == formatter_param_cb_map().end()) {
          META_ERROR_FMT("Illegal param: %s at %d", pattern[j], j);
          break;
        }
        _pattern.push_back(res->second(EMPTY_PARAM));
      }
    }
#endif
  }
}

void LogFormatter::Append(const char *key, std::function<LogFormatterParam::ptr(const std::string &)> param) {
  if (formatter_param_cb_map().find(key) != formatter_param_cb_map().end()) {
    META_WARN_FMT("The Key \"%s\" has already exist in callback map!", key);
    return;
  }
  formatter_param_cb_map().insert(std::pair<const char *,
                              std::function<LogFormatterParam::ptr(const std::string &)>>(key, param));
}

void LogFormatter::Remove(const std::string& key) {
  formatter_param_cb_map().erase(key);
}

void LogConsoleOutput::Print(Logger::ptr log, LogEvent::ptr event) {
  for (auto &i : _formatter->pattern()) {
    i->Format(std::cout, log, event);
  }
  std::cout << std::endl;
}

LogWrap::LogWrap(const Logger::ptr &logger, LogEvent::ptr event) : 
                _logger(logger), _event(event) {}

LogWrap::~LogWrap() {
  _logger->Print(_event);
}

static std::shared_ptr<Logger> CreateLogger(LogLevel::level level = meta::LogLevel::INFO){
  auto f = std::make_shared<LogFormatter>();
  // TODO:Get it from CFG & Add file output
  f->Init(DEFAULT_FORMATTER_PATTERN);
  auto logger = std::make_shared<Logger>(level);
  auto o = std::make_shared<LogConsoleOutput>(f);
  logger->AppendOutput(o);
  return logger;
}

LoggerManager::LoggerManager() : _default_logger(std::make_shared<Logger>()) {
  CreateLogger();
}

Logger::ptr LoggerManager::GetLogger(LogLevel::level level) {
  auto res = k_logger_mgr.find(level);
  if (res == k_logger_mgr.end()) {
    // Crated it if it's not exist
    auto logger = CreateLogger(level);
    k_logger_mgr.insert(std::pair<LogLevel::level, Logger::ptr>(level, logger));
    return logger;
  }
  return res->second;
}

void LoggerManager::Append(LogLevel::level level, Logger::ptr log) {
  if (k_logger_mgr.find(level) != k_logger_mgr.end()) {
    META_WARN_FMT("The logger of %s is already exist!", LogLevel::ToString(level));
    return;
  }
  k_logger_mgr.insert(std::pair<LogLevel::level, Logger::ptr>(level, log));
}

void LoggerManager::Remove(LogLevel::level level) {
  k_logger_mgr.erase(level);
}

} // namespace meta