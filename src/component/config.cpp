#include "config.h"

#include <iostream>

#define CONFIG_ARG(Key) cfg, #Key, m_config.Key

namespace com {

Config* Config::inst = nullptr;

Config::Config() {
  YAML::Node p_config;
  try {
    p_config = YAML::LoadFile("config.yaml");
    Read(p_config);
  } catch (YAML::Exception e) {
    std::cout << e.msg << std::endl;
  }
}

void Config::Read(YAML::Node& cfg) {
  // 目录
  ReadOption<std::string>(CONFIG_ARG(directory));
  // 端口
  ReadOption<u_int32_t>(CONFIG_ARG(port));
  // 定时器检查连接时长
  ReadOption<u_int32_t>(CONFIG_ARG(timer_time_out));
  // 连接保活时长
  ReadOption<u_int32_t>(CONFIG_ARG(client_alive_time));
  // 客户端容量
  ReadOption<u_int32_t>(CONFIG_ARG(client_capacity));
  // 读取缓存区大小
  ReadOption<u_int32_t>(CONFIG_ARG(read_buf_length));
}

template <typename T>
void Config::ReadOption(YAML::Node& cfg, const char* key, T& dst) {
  if (cfg[key].Type() != YAML::NodeType::Undefined) dst = cfg[key].as<T>();
}

}  // namespace com