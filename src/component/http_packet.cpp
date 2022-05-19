#include "http_packet.h"

#include <sys/mman.h>

#include <iostream>

#include "config.h"
#include "router.h"

namespace com {
namespace http {

// 返回文件或者数据
Packet::Packet(std::string _version, std::string data, Type _type)
    : m_version(_version), m_data(data), m_type(_type) {}

// 返回错误请求
Packet::Packet(std::string _version, HTTP_CODE code)
    : m_version(_version), m_code(code) {
  m_type = CODE;
}

void Packet::AddHeader(std::string key, std::string value) {
  m_header[key] = value;
}

void Packet::Process() {
  MakeInfoAndHeader();

  switch (m_type) {
    case CODE: {
      auto len = k_http_code_dict[m_code].size();
      char* content = nullptr;
      content = new char[96 + len];

      sprintf(content, RESPONSE_CONTENT, m_code,
              k_http_code_dict[m_code].c_str());
      m_buf += content;
      break;
    }
    case FILE: {
      ReadFile();
      m_buf += (char*)m_file_address;
      ReleaseFile();
      break;
    }
    case DATA: {
      m_buf += m_data;
      break;
    }
  }
}

void Packet::MakeInfoAndHeader() {
  // Info
  if (m_type == CODE) {
    m_buf += m_version + " " + std::to_string(m_code) + " " +
             k_http_code_dict[m_code] + "\r\n";
  } else {
    m_buf += m_version + " 200 OK\r\n";
  }

  // Header
  for (auto i : m_header) {
    m_buf += i.first + ": " + i.second + "\r\n";
  }
  m_buf += "\r\n";
}

// 读取文件
void Packet::ReadFile() {
  m_file_size = com::Router::Instance()->file_list(m_data).file_stat.st_size;
  std::string path = com::Config::Instance()->config().directory + m_data;

  auto fd = open(path.c_str(), O_RDONLY);
  m_file_address = mmap(nullptr, m_file_size, PROT_READ, MAP_PRIVATE, fd, 0);

  close(fd);
}

// 释放文件
void Packet::ReleaseFile() { munmap(m_file_address, m_file_size); }

}  // namespace http
}  // namespace com