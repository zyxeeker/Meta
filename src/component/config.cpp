#include "config.h"

#include <iostream>

#define CONFIG_ARG(Key) #Key, m_config.Key

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
  ReadOption<std::string>(cfg, CONFIG_ARG(directory));
  // 端口
  ReadOption<u_int32_t>(cfg, CONFIG_ARG(port));
}

template <typename T>
void Config::ReadOption(YAML::Node& cfg, const char* key, T& dst) {
  if (cfg[key].Type() != YAML::NodeType::Undefined) dst = cfg[key].as<T>();
}

}  // namespace com