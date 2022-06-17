#include "http_parse.h"

#include <cstring>
#include <exception>
#include <iostream>

namespace com {
namespace http {

int Parse::Process(size_t len) {
  m_len = len + m_offset;
  auto line_check_state = LINE_OK;

  while (m_check_state == CHECK_CONTENT ||
         ((line_check_state = ParseLine()) == LINE_OK && m_right < m_len)) {
    switch (m_check_state) {
      // 解析请求体
      case CHECK_REQUEST: {
        auto res = ParseRequest();
        if (res == BAD_METHOD) return METHOD_NOT_ALLOWED;
        if (res == BAD_VERSION) return HTTP_VERSION_NOT_SUPPORTED;

        m_check_state = CHECK_HEADER;
        break;
      }
      // 解析请求头部
      case CHECK_HEADER: {
        if (m_right - m_left <= 2) {
          m_check_state = CHECK_CONTENT;
          break;
        }
        if (ParseHeader() != LINE_OK) return BAD_REQUEST;
        break;
      }
      // 解析请求内容
      case CHECK_CONTENT: {
        if (m_header["Content-Length"] == "") return OK;

        m_content_length = atoi(m_header["Content-Length"].c_str());
        if (ParseContent() == LINE_OK) return OK;

        m_offset = m_len;
        return -1;
      }
    }
    m_right += 2;
    m_left = m_right;

    m_offset = m_left;
    // 判断请求头是否解析完毕
    if (m_left + 1 >= m_len) {
      m_offset = m_len;
      return -1;
    } else if (m_data[m_left] == '\r' && m_data[m_left + 1] == '\n') {
      // 判断是否有请求内容
      if (m_header["Content-Length"] == "")
        return OK;
      else {
        m_check_state = CHECK_CONTENT;
        m_left += 2;
      }
    }
  }

  if (line_check_state == LINE_OPEN) {
    m_offset = m_len;
    return -1;
  }

  if (line_check_state == LINE_BAD) return BAD_REQUEST;

  return OK;
}

// 解析\r\n标志位
Parse::LineResult Parse::ParseLine() {
  for (int index = m_offset; index < m_len; ++index) {
    if (m_data[index] == '\r') {
      if (index + 1 >= m_len) break;
      if (m_data[index + 1] == '\n') {  // 完整一行
        m_right = index;
        return LINE_OK;
      }
      return LINE_BAD;  // 该行出错
    } else if (m_data[index] == '\n') {
      if (index - 1 >= 0 && m_data[index - 1] == '\r') {  // 完整一行
        m_right = --index;
        return LINE_OK;
      }
      return LINE_BAD;  // 该行出错
    }
  }
  return LINE_OPEN;  // 该行不完整
}

// 请求体解析
Parse::RequestResult Parse::ParseRequest() {
  auto check_request_state = CHECK_METHOD;
  int start = 0;
  for (int cur = 0; cur <= m_right; ++cur) {
    if (m_data[cur] == ' ' || m_data[cur] == '\r') {
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
Parse::LineResult Parse::ParseHeader() {
  char* p_data = m_data + m_left;
  for (int cur = 0; cur < m_right - m_left; ++cur) {
    if (p_data[cur] == ':' && p_data[cur + 1] == ' ') {
      m_header[std::string(p_data, cur)] =
          std::string(p_data + cur + 2, (m_right - m_left) - cur - 2);

      return LINE_OK;
    }
  }
  return LINE_BAD;
}

// 判断请求内容是否完整
Parse::LineResult Parse::ParseContent() {
  auto end = m_left + m_content_length;
  if (end > m_len) return LINE_OPEN;

  if (m_data[end] == '\0') {
    m_content = std::string(m_data + m_left, m_content_length);
    return LINE_OK;
  }
  return LINE_OPEN;
}

// 判断请求方式
HttpMethod Parse::JudgeMethod(const size_t& end) {
  if (strncmp(m_data, "GET", end) == 0) {
    return GET;
  }
  if (strncmp(m_data, "HEAD", end) == 0) {
    return HEAD;
  }
  if (strncmp(m_data, "POST", end) == 0) {
    return POST;
  }
  if (strncmp(m_data, "PUT", end) == 0) {
    return PUT;
  }
  if (strncmp(m_data, "DELETE", end) == 0) {
    return DELETE;
  }
  if (strncmp(m_data, "CONNECT", end) == 0) {
    return CONNECT;
  }
  if (strncmp(m_data, "OPTIONS", end) == 0) {
    return OPTIONS;
  }
  if (strncmp(m_data, "TRACE", end) == 0) {
    return TRACE;
  }
  if (strncmp(m_data, "PATCH", end) == 0) {
    return PATCH;
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