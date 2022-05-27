#ifndef SRC_LOGGER_LOG_H_
#define SRC_LOGGER_LOG_H_

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
  char date[50]{0};
  char file_level_buf[10]{0};
  char level_buf[20]{0};
  std::string buffer;
};

}  // namespace logger

#endif  // SRC_LOGGER_LOG_H_
