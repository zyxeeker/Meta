#ifndef SRC_COMMON_UTIL_H_
#define SRC_COMMON_UTIL_H_

#include <fcntl.h>

namespace util {

// 设置fd为非阻塞
static void SetNoBlock(int& fd) {
  int old_options = fcntl(fd, F_GETFL);
  int new_options = old_options | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_options);
}

}  // namespace util

#endif  // SRC_COMMON_UTIL_H_
