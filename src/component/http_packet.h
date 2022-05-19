#ifndef SRC_COMPONENT_HTTP_PACKET_H_
#define SRC_COMPONENT_HTTP_PACKET_H_

#include <string>
#include <unordered_map>

#include "http.h"

namespace com {
namespace http {

class Packet {
 public:
  enum Type { CODE, FILE, DATA };
  // 返回文件请求
  Packet(std::string _version, std::string data, Type _type);
  // 返回错误请求
  Packet(std::string _version, HTTP_CODE code);

  void Process();

  void AddHeader(std::string key, std::string value);
  // 包装类型
  Type type() { return m_type; }

  const char* buf() const { return m_buf.c_str(); }

  size_t size() const { return m_buf.size(); }

 private:
  void MakeInfoAndHeader();
  // 读取文件
  void ReadFile();
  // 释放文件
  void ReleaseFile();

 private:
  Type m_type;
  int m_code = 200;

  std::string m_version;
  std::unordered_map<std::string, std::string> m_header;

  // 存储文件或者数据
  std::string m_data;

  std::string m_buf;

  // 文件地址
  void* m_file_address = nullptr;
  // 文件大小
  size_t m_file_size;
};

}  // namespace http
}  // namespace com

#endif  // SRC_COMPONENT_HTTP_PACKET_H_
