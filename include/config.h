#ifndef META_INCLUDE_CONFIG_H_
#define META_INCLUDE_CONFIG_H_

#include <string>
#include <vector>

namespace meta {
// 序列化
template<typename S, typename D>
class Convert {
 public:
  // TODO
};

// 反序列化
template<typename D, typename S>
class Deconvert {
// TODO
};

// 配置基础类型
template<typename T>
class CfgBasicVar {
 public:
  CfgBasicVar(T val) : _val(val) {}
  virtual std::string ToString() = 0;
  T GetValue() const { return _val; }
 protected:
  T _val;
};

class CfgInt : public CfgBasicVar<int> {
 public:
  CfgInt(int val) : CfgBasicVar<int>(val) {}
  virtual std::string ToString() override {
    // TODO
    return std::to_string(_val);
  }
};

class ConfigManager {
 public:
  ConfigManager();
};

}

#endif // META_INCLUDE_CONFIG_H_