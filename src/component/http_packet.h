#ifndef SRC_COMPONENT_HTTP_PACKET_H_
#define SRC_COMPONENT_HTTP_PACKET_H_

#include <sys/uio.h>

#include <cstdio>
#include <string>
#include <unordered_map>

#include "http.h"

namespace com {
namespace http {

class Packet {
 public:
  enum Type { CODE, FILE, DATA };

  Packet() : m_buf_size(1024) {
    m_buf = new char[m_buf_size];
    bzero(m_buf, m_buf_size);
  }
  ~Packet();

  // 重置
  void Reset();

  // 数据or文件
  void Process(std::string data, Type type);
  // 错误代码
  void Process(HttpCode code);

  // 释放文件
  void ReleaseFile();

  // Http版本
  void set_version(HttpVersion version) { VersionTrans(version); }
  std::string version() const { return m_version; }

  // buffer大小
  size_t buf_size() const { return m_buf_size; }
  // 文件大小
  size_t file_size() const { return m_file_size; }

  // 包装类型
  Type type() { return m_type; }
  // 文件iov结构体
  iovec* file_buf() { return m_file_buf; }
  // 文件地址
  void* file_addr() const { return m_file_address; }

  const char* buf() { return m_buf; }

 private:
  // 读取文件
  void ReadFile();
  // 版本解析
  void VersionTrans(HttpVersion _version);
  // 数据处理
  void Process();
  // 写入状态行
  void AddStatueLine();
  // 添加头部选项
  void AddHeader(const char* key, const char* value);
  void AddHeader(const char* key, int value);
  // 添加空行
  void AddBkLine();
  // 添加内容
  void AddContent();
  // 扩充buffer大小
  void ExpandBuffer(int remain_size = 0);
  // 写入buffer函数
  bool WriteBuffer(const char* format, ...);

 private:
  // 响应类型
  Type m_type;
  // HttpCode
  int m_code = 200;
  // Http版本
  std::string m_version = "HTTP/1.1";

  char* m_buf = nullptr;

  // 写入偏移
  int m_buf_offset = 0;
  // buffer大小
  int m_buf_capacity = 1024;
  // 实际大小
  size_t m_buf_size = 0;

  // buffer大小增长倍率
  float m_buf_size_rate = 0.4;

  // 存储文件或者数据
  std::string m_data;

  // 文件地址
  void* m_file_address = nullptr;
  // 文件大小
  size_t m_file_size = 0;

  struct iovec m_file_buf[2];
};

}  // namespace http
}  // namespace com

#endif  // SRC_COMPONENT_HTTP_PACKET_H_
