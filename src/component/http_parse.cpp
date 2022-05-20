#include "http_parse.h"

#include <cstring>
#include <exception>
#include <iostream>

namespace com {
namespace http {

Parse::Parse(char* _data, size_t _len) : m_data(_data), m_len(_len) {}

HttpCode Parse::Process() {
  size_t left = 0, right = 0;
  auto check_state = CHECK_REQUEST;

  while (check_state == CHECK_CONTENT ||
         (ParseLine(left, right) == LINE_OK && right < m_len)) {
    switch (check_state) {
      // 解析请求体
      case CHECK_REQUEST: {
        auto res = ParseRequest(right);
        if (res == BAD_METHOD) return METHOD_NOT_ALLOWED;
        if (res == BAD_VERSION) return HTTP_VERSION_NOT_SUPPORTED;

        check_state = CHECK_HEADER;
        break;
      }
      // 解析请求头部
      case CHECK_HEADER: {
        if (ParseHeader(left, right) != LINE_OK) return BAD_REQUEST;
        break;
      }
      // 解析请求内容
      case CHECK_CONTENT: {
        if (m_header["Content-Length"] == "") return OK;
        m_content_length = atoi(m_header["Content-Length"].c_str());
        if (ParseContent(left) == LINE_OK) return OK;
      }
    }
    right += 2;
    left = right;
    // 判断是否有请求内容
    if (m_data[left] == '\r' && m_data[left + 1] == '\n') {
      check_state = CHECK_CONTENT;
      left += 2;
    }
  }
  return OK;
}

// 解析\r\n标志位
Parse::LineResult Parse::ParseLine(size_t& left, size_t& right) {
  for (; right < m_len; ++right) {
    if (m_data[right] == '\r') {
      if (m_data[right + 1] == '\n') {  // 完整一行
        m_data[right] = '\0';
        m_data[right + 1] = '\0';
        return LINE_OK;
      } else
        return LINE_OPEN;  // 该行不完整
    }
  }
  return LINE_BAD;
}

// 请求体解析
Parse::RequestResult Parse::ParseRequest(const size_t& right) {
  auto check_request_state = CHECK_METHOD;
  int start = 0;
  for (int cur = 0; cur <= right; ++cur) {
    if (m_data[cur] == ' ' || m_data[cur] == '\0') {
      switch (check_request_state) {
        case CHECK_METHOD: {
          if ((m_method = JudgeMethod(cur)) == UNKNOWN) return BAD_METHOD;
          check_request_state = CHECK_URL;
          start = cur + 1;
          break;
        }
        case CHECK_URL: {
          m_url = std::string(m_data + start, cur - start);
          check_request_state = CHECK_VERSION;
          start = cur + 1;
          break;
        }
        case CHECK_VERSION: {
          if ((m_version = JudgeVersion(start, cur)) == UNSUPPORTED)
            return BAD_VERSION;
          break;
        }
      }
    }
  }
  return REQUEST_OK;
}

// 请求头部解析
Parse::LineResult Parse::ParseHeader(const size_t& left, const size_t& right) {
  int cur = 0;
  char* p_data = m_data + left;
  for (; cur < right; ++cur) {
    if (p_data[cur] == ':' && p_data[cur + 1] == ' ') {
      m_header[std::string(p_data, cur)] =
          std::string(p_data + cur + 2, (right - left) - cur - 2);

      return LINE_OK;
    }
  }
  return LINE_BAD;
}

// 判断请求内容是否完整
Parse::LineResult Parse::ParseContent(size_t& left) {
  char* p = m_data + left;
  if (m_data[left + m_content_length] == '\0') {
    m_content = std::string(m_data + left, m_content_length);
    return LINE_OK;
  }
  return LINE_OPEN;
}

// 判断请求方式
HttpMethod Parse::JudgeMethod(const size_t& end) {
  if (strncmp(m_data, "GET", end) == 0) {
    return GET;
  }
  return UNKNOWN;
}

// 判断Http版本
HttpVersion Parse::JudgeVersion(const size_t& start, const size_t& end) {
  if (strncmp(m_data + start, "HTTP/1.1", end - start) == 0) {
    return HTTP_1_1;
  }
  return UNSUPPORTED;
}

}  // namespace http
}  // namespace com