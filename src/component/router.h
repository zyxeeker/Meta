#ifndef SRC_COMPONENT_ROUTER_H_
#define SRC_COMPONENT_ROUTER_H_

#include <fcntl.h>
#include <sys/dir.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "common/marco.h"

namespace com {

class Router {
 public:
  typedef struct file_info_ {
    // 文件信息
    struct stat file_stat;
  } FILE_INFO;

  static Router* Instance() {
    if (!inst) inst = new Router();
    return inst;
  }

  void Process();
  const FILE_INFO& file_list(std::string key) { return m_table[key]; }

 private:
  Router();
  ~Router() = default;

  static Router* inst;

  std::unordered_map<std::string, FILE_INFO> m_table;

  DISALLOW_COPY_AND_ASSIGN(Router);
};

}  // namespace com

#endif  // SRC_COMPONENT_ROUTER_H_
