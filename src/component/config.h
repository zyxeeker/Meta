#ifndef SRC_COMPONENT_CONFIG_H_
#define SRC_COMPONENT_CONFIG_H_

#include <yaml-cpp/yaml.h>

#include <string>
#include <unordered_map>

#include "component/http.h"

namespace com {

class Config {
 public:
  typedef struct reverse_proxy {
    std::string dst = "";           // 反代目标
    int port = 80;                  // 反代目标端口
    int allow_method = ALL_METHOD;  // 允许的方式
  } ReverseProxy;

  typedef struct config {
    std::string directory = "/home/www";  // 静态目录
    u_int32_t port = 9006;                // 端口
    u_int32_t timer_time_out = 180;       // 定时器检查连接时长
    u_int32_t client_alive_time = 180;    // 连接保活时长
    u_int32_t client_capacity = 50;       // 客户端容量
    u_int32_t read_buf_length = 4096;     // 读取缓存区大小
    std::unordered_map<std::string, ReverseProxy*> proxy_settings;  // 反代字典
  } CFG;

  static Config* Instance() {
    if (!inst) inst = new Config();
    return inst;
  }

  const CFG& config() { return m_config; }

 private:
  Config();

  void Read(YAML::Node& cfg);
  template <typename T>
  inline void ReadOption(YAML::Node& cfg, const char* key, T& dst);

  inline void ReadRedirectSettings(YAML::Node& cfg);

 private:
  static Config* inst;

  CFG m_config;
};

}  // namespace com

#endif  // SRC_COMPONENT_CONFIG_H_
