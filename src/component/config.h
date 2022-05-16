#ifndef SRC_COMPONENT_CONFIG_H_
#define SRC_COMPONENT_CONFIG_H_

#include <yaml-cpp/yaml.h>

#include <string>

namespace com {

class Config {
 public:
  typedef struct config {
    std::string directory = "/home/www";
    u_int32_t port = 9006;
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

 private:
  static Config* inst;

  CFG m_config;
};

}  // namespace com

#endif  // SRC_COMPONENT_CONFIG_H_
