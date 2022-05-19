#include "router.h"

#include <unistd.h>

#include <cstring>
#include <deque>
#include <iostream>

#include "config.h"

namespace com {

Router *Router::inst = nullptr;

Router::Router() { Process(); }

void Router::Process() {
  std::string home = com::Config::Instance()->config().directory;
  // 配置中存在静态项目目录，则读取管理静态文件的信息
  if (!home.empty()) {
    std::deque<std::string> path;
    path.push_back("/");

    struct direct **name_list = nullptr;

    while (path.size() != 0) {
      auto p = path.front();
      auto scan_path = home + p;
      int n = scandir(scan_path.c_str(), &name_list, nullptr, alphasort);

      if (n < 0) return;

      while (n--) {
        if (strcmp(name_list[n]->d_name, ".") &&
            strcmp(name_list[n]->d_name, "..")) {
          // 是目录则存进队列
          if (name_list[n]->d_type == DT_DIR) {
            path.push_back(p + name_list[n]->d_name + '/');
            continue;
          }
          // 是文件则记录信息
          if (name_list[n]->d_type == DT_REG) {
            FILE_INFO *p_info = &m_table[p + name_list[n]->d_name];
            auto hr = stat((scan_path + "/" + name_list[n]->d_name).c_str(),
                           &(p_info->file_stat));

            if (hr == -1) {
              // TODO
              return;
            }
          }
        }
        free(name_list[n]);
      }
      path.pop_front();
    }
  } else {
    // TODO: 未设置静态项目目录的情况
  }
}

const Router::FILE_INFO *Router::Find(std::string key) {
  auto it = m_table.find(key);
  if (it == m_table.end()) return nullptr;

  return &(it->second);
}

}  // namespace com