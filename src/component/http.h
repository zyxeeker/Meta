#ifndef SRC_COMPONENT_HTTP_H_
#define SRC_COMPONENT_HTTP_H_

#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <unordered_map>

#define ALL_METHOD 0x01FF

namespace com {
namespace http {

enum HttpVersion { UNSUPPORTED = -1, HTTP_1_1 };

enum HttpMethod {
  UNKNOWN = 0x0000,
  GET = 0x0001,
  HEAD = 0x0002,
  POST = 0x0004,
  PUT = 0x0010,
  DELETE = 0x0020,
  CONNECT = 0x0040,
  OPTIONS = 0x0080,
  TRACE = 0x0100,
  PATCH = 0x0200
};

const int HTTP_CODE_CONTENT_LENGTH = 96;
const char HTTP_CODE_CONTENT[] =
    "<head><title>%d</title></head>"
    "<body><center><h1>%s</h1></center>"
    "<hr/><center>Meta</center></body>";

enum HttpCode {
  // 1xx
  CONTINUE = 100,
  SWITCHING_PROTOCOLS = 101,
  // 2xx
  OK = 200,
  CREATED = 201,
  ACCEPTED = 202,
  NON_AUTHORITATIVE_INFORMATION = 203,
  NO_CONTENT = 204,
  RESET_CONTENT = 205,
  PARTIAL_CONTENT = 206,
  // 3xx
  MULTIPLE_CHOICES = 300,
  MOVED_PERMANENTLY = 301,
  FOUND = 302,
  SEE_OTHER = 303,
  NOT_MODIFIED = 304,
  USE_PROXY = 305,
  UNUSED = 306,
  TEMPORARY_REDIRECT = 307,
  // 4xx
  BAD_REQUEST = 400,
  UNAUTHORIZED = 401,
  PAYMENT_REQUIRED = 402,
  FORBIDDEN = 403,
  NOT_FOUND = 404,
  METHOD_NOT_ALLOWED = 405,
  NOT_ACCEPTABLE = 406,
  PROXY_AUTHENTICATION_REQUIRED = 407,
  REQUEST_TIME_OUT = 408,
  CONFLICT = 409,
  GONE = 410,
  LENGTH_REQUIRED = 411,
  PRECONDITION_FAILED = 412,
  REQUEST_ENTITY_TOO_LARGE = 413,
  REQUEST_URI_TOO_LARGE = 414,
  UNSUPPORTED_MEDIA_TYPE = 415,
  REQUESTED_RANGE_NOT_SATISFIABLE = 416,
  EXPECTATION_FAILED = 417,
  // 5xx
  INTERNAL_SERVER_ERROR = 500,
  NOT_IMPLEMENTED = 501,
  BAD_GATEWAY = 502,
  SERVICE_UNAVAILABLE = 503,
  GATEWAY_TIME_OUT = 504,
  HTTP_VERSION_NOT_SUPPORTED = 505
};

static std::unordered_map<int, std::string> k_http_code_dict{
    // 1xx
    {100, "Continue"},
    {101, "Switching Protocols"},
    // 2xx
    {200, "OK"},
    {201, "Created"},
    {202, "Accepted"},
    {203, "Non-Authoritative Information"},
    {204, "No Content"},
    {205, "Reset Content"},
    {206, "Partial Content"},
    // 3xx
    {300, "Multiple Choices"},
    {301, "Moved Permanently"},
    {302, "Found"},
    {303, "See Other"},
    {304, "Not Modified"},
    {305, "Use Proxy"},
    {306, "Unused"},
    {307, "Temporary Redirect"},
    // 4xx
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {402, "Payment Required"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {406, "Not Acceptable"},
    {407, "Proxy Authentication Required"},
    {408, "Request Time-out"},
    {409, "Conflict"},
    {410, "Gone"},
    {411, "Length Required"},
    {412, "Precondition Failed"},
    {413, "Request Entity Too Large"},
    {414, "Request-URI Too Large"},
    {415, "Unsupported Media Type"},
    {416, "Requested range not satisfiable"},
    {417, "Expectation Failed"},
    // 5xx
    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Time-out"},
    {505, "HTTP Version not supported"}};

}  // namespace http
}  // namespace com

#endif  // SRC_COMPONENT_HTTP_H_
