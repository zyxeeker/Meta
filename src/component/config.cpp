#include "config.h"

#include <iostream>

#include "component/http.h"

#define CONFIG_ARG(Key) cfg, #Key, m_config.Key

namespace com {

static http::HttpMethod JudgeMethod(std::string& data) {
  if (data == "GET") return http::GET;
  if (data == "HEAD") return http::HEAD;
  if (data == "POST") return http::POST;
  if (data == "PUT") return http::PUT;
  if (data == "DELETE") return http::DELETE;
  if (data == "CONNECT") return http::CONNECT;
  if (data == "OPTIONS") return http::OPTIONS;
  if (data == "TRACE") return http::TRACE;
  if (data == "PATCH") return http::PATCH;
}

Config* Config::inst = nullptr;

Config::Config() {
  YAML::Node p_config;
  try {
    p_config = YAML::LoadFile("config.yaml");
    Read(p_config);
    ReadRedirectSettings(p_config);
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

void Config::ReadRedirectSettings(YAML::Node& cfg) {
  auto data = cfg["url_redirect"];
  if (data.Type() != YAML::NodeType::Undefined) {
    for (auto it : data) {
      if (it["src"].Type() == YAML::NodeType::Undefined &&
          it["dst"].Type() == YAML::NodeType::Undefined)
        continue;

      auto proxy_cfg = new UrlRedirectConfig;
      if (it["allow_method"].Type() != YAML::NodeType::Undefined) {
        proxy_cfg->allow_method = http::UNKNOWN;
        for (auto method : it["allow_method"]) {
          auto t = method.as<std::string>();
          proxy_cfg->allow_method |= JudgeMethod(t);
        }
      }

      if (it["port"].Type() != YAML::NodeType::Undefined)
        proxy_cfg->port = it["port"].as<int32_t>();

      m_config.redirect_settings[it["src"].as<std::string>()] = proxy_cfg;
    }
  }
}

}  // namespace com