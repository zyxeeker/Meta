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

  Parse(char* _data, size_t _len);

  HTTP_CODE Process();

  // Http method
  const Method& method() { return m_method; }
  // 请求url
  const std::string& url() { return m_url; }
  // Http version
  const std::string& version() { return m_version; }
  // 请求头部选项
  const std::unordered_map<std::string, std::string>& header() {
    return m_header;
  }
  // 请求体长度
  const int& content_length() { return m_content_length; }
  // 请求体
  const std::string& content() { return m_content; }

 private:
  // 解析\r\n标志位
  LineResult ParseLine(size_t& left, size_t& right);
  // 处理请求体
  LineResult ParseRequest(const size_t& right);
  // 处理请求头部选项
  LineResult ParseHeader(const size_t& left, const size_t& right);
  // 判断请求内容是否完整
  LineResult ParseContent(size_t& left);
  // 判断请求方式
  Method JudgeMethod(const size_t& start, const size_t& end);

 private:
  char* m_data = nullptr;
  size_t m_len;

  Method m_method;
  std::string m_url;
  std::string m_version;
  std::unordered_map<std::string, std::string> m_header;

  int m_content_length;
  std::string m_content;
};

}  // namespace http
}  // namespace com

#endif  // SRC_COMPONENT_HTTP_PARSE_H_
