#ifndef SRC_COMPONENT_HTTP_PARSE_H_
#define SRC_COMPONENT_HTTP_PARSE_H_

#include <sys/types.h>

#include <string>
#include <unordered_map>

#include "http.h"

namespace com {
namespace http {

class Parse {
 public:
  enum CheckState { CHECK_REQUEST, CHECK_HEADER, CHECK_CONTENT };
  enum CheckRequestState { CHECK_METHOD, CHECK_URL, CHECK_VERSION };
  enum LineResult { LINE_BAD = -1, LINE_OK, LINE_OPEN };
  enum RequestResult { REQUEST_BAD = -1, REQUEST_OK, BAD_VERSION, BAD_METHOD };

  Parse(char* _data) : m_data(_data) {}

  int Process(size_t len);

  // Http method
  const HttpMethod& method() { return m_method; }
  // 请求url
  const std::string& url() { return m_url; }
  // Http version
  const HttpVersion& version() { return m_version; }
  // 请求头部选项
  const std::unordered_map<std::string, std::string>& header() {
    return m_header;
  }
  // 请求体长度
  const int& content_length() { return m_content_length; }
  // 请求体
  const std::string& content() { return m_content; }
  // 原始数据
  const char* data() { return m_data; }
  // 原始数据长度
  size_t len() const { return m_len; }

 private:
  // 解析\r\n标志位
  LineResult ParseLine();
  // 处理请求体
  RequestResult ParseRequest();
  // 处理请求头部选项
  LineResult ParseHeader();
  // 判断请求内容是否完整
  LineResult ParseContent();
  // 判断请求方式
  HttpMethod JudgeMethod(const size_t& end);
  // 判断Http版本
  HttpVersion JudgeVersion(const size_t& start, const size_t& end);

 private:
  char* m_data = nullptr;
  // buffer长度
  size_t m_len = 0;
  // 偏移值
  size_t m_offset = 0;
  // 窗口左边界
  size_t m_left = 0;
  // 窗口右边界
  size_t m_right = 0;

  CheckState m_check_state = CHECK_REQUEST;

  HttpMethod m_method;
  std::string m_url;
  // std::string m_version;
  HttpVersion m_version;
  std::unordered_map<std::string, std::string> m_header;

  int m_content_length;
  std::string m_content;
};

}  // namespace http
}  // namespace com

#endif  // SRC_COMPONENT_HTTP_PARSE_H_
