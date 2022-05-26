#include "http_packet.h"

#include <sys/mman.h>

#include <cstdarg>

#include "config.h"
#include "router.h"

namespace com {
namespace http {

Packet::~Packet() {
  ReleaseFile();
  delete[] m_buf;
}

// 数据or文件
void Packet::Process(std::string data, Type type) {
  m_type = type;
  m_data = data;
  Process();
}
// 错误代码
void Packet::Process(HttpCode code) {
  m_type = CODE;
  m_code = code;
  Process();
}

void Packet::Process() {
  switch (m_type) {
    case CODE: {
      AddStatueLine();
      AddHeader("Connection", "keep-alive");
      AddHeader("Content-Length",
                HTTP_CODE_CONTENT_LENGTH + k_http_code_dict[m_code].size());
      AddBkLine();
      AddContent();
      m_buf_size = m_buf_offset + HTTP_CODE_CONTENT_LENGTH +
                   k_http_code_dict[m_code].size();
      break;
    }
    case FILE: {
      ReadFile();
      AddStatueLine();
      AddHeader("Content-Length", m_file_size);
      AddBkLine();

      m_file_buf[0].iov_base = m_buf;
      m_file_buf[0].iov_len = m_buf_offset;
      m_file_buf[1].iov_base = m_file_address;
      m_file_buf[1].iov_len = m_file_size;

      m_buf_size = m_buf_offset;
      break;
    }
    case DATA: {
      AddStatueLine();
      AddHeader("Content-Length", m_data.size());
      AddBkLine();
      AddContent();

      m_buf_size = m_buf_offset + m_data.size();
      break;
    }
  }
}

// 读取文件
void Packet::ReadFile() {
  if (m_file_address) ReleaseFile();

  m_file_size = com::Router::Instance()->Find(m_data)->file_stat.st_size;
  std::string path = com::Config::Instance()->config().directory + m_data;

  auto fd = open(path.c_str(), O_RDONLY);
  m_file_address = mmap(nullptr, m_file_size, PROT_READ, MAP_PRIVATE, fd, 0);

  close(fd);
}

// 释放文件
void Packet::ReleaseFile() { munmap(m_file_address, m_file_size); }

// 版本解析
void Packet::VersionTrans(HttpVersion _version) {
  switch (_version) {
    case HTTP_1_1: {
      m_version = "HTTP/1.1";
    }
  }
}

// 写入状态行
void Packet::AddStatueLine() {
  WriteBuffer("%s %d %s\r\n", m_version.c_str(), m_code,
              k_http_code_dict[m_code].c_str());
}
// 添加头部选项
void Packet::AddHeader(const char* key, const char* value) {
  WriteBuffer("%s: %s\r\n", key, value);
}
void Packet::AddHeader(const char* key, int value) {
  WriteBuffer("%s: %d\r\n", key, value);
}
// 添加空行
void Packet::AddBkLine() { WriteBuffer("\r\n"); }
// 添加内容
void Packet::AddContent() {
  if (m_type == CODE) {
    WriteBuffer(HTTP_CODE_CONTENT, m_code, k_http_code_dict[m_code].c_str());
  } else if (m_type == DATA) {
    WriteBuffer("%s", m_data.c_str());
  } else {
    WriteBuffer("%s", m_file_address);
  }
}

// 扩充buffer大小
void Packet::ExpandBuffer(int remain_size) {
  m_buf_capacity = m_buf_capacity * (1 + m_buf_size_rate);
  char* tmp = new char[m_buf_capacity];
  bzero(tmp, m_buf_capacity);
  memcpy(tmp, m_buf, m_buf_capacity - remain_size + 1);
  delete[] m_buf;
  m_buf = tmp;
}

// 写入buffer函数
bool Packet::WriteBuffer(const char* format, ...) {
  if (m_buf_offset >= m_buf_capacity) ExpandBuffer();

  va_list args;
  va_start(args, format);
  // 要写入长度 剩余空间
  int len = 0, remain_size;
  do {
    remain_size = m_buf_capacity - m_buf_offset - 1;
    len = vsnprintf(m_buf + m_buf_offset, m_buf_capacity - m_buf_offset - 1,
                    format, args);
    // 判断是否足够写入 当写入空间不足够时按倍率增加空间
    if (len >= remain_size) ExpandBuffer(remain_size);

  } while (len >= remain_size);

  m_buf_offset += len;
  va_end(args);
  return true;
}
}  // namespace http
}  // namespace com