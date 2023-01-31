#include <pthread.h>
#include <unistd.h>

#include <iostream>

#include "component/config.h"
#include "component/http_packet.h"
#include "component/http_parse.h"
#include "component/router.h"
#include "gtest/gtest.h"
#include "logger/core.h"
#include "net/core.h"
#include "thread/pool.h"
#include "logger/log.h"

#define META_LOOP 1

// HTTP解析
TEST(Http, Parse) {
  char request[] =
      "GET url/se/index/ssss HTTP/1.1\r\n"
      "Host: localhost:8080\r\n"
      "Content-Length: 6\r\n"
      "\r\n"
      "asasaa";
  com::http::Parse parse(request);
  parse.Process(strlen(request));
  EXPECT_EQ(parse.method(), com::http::GET);
  EXPECT_EQ(parse.url(), "url/se/index/ssss");
  EXPECT_EQ(parse.version(), com::http::HTTP_1_1);
  EXPECT_EQ(parse.header().at("Host"), "localhost:8080");
  EXPECT_EQ(parse.header().at("Content-Length"), "6");
  EXPECT_EQ(parse.content_length(), 6);
  EXPECT_EQ(parse.content(), "asasaa");
}

TEST(Http, Parse_LineOpen) {
  char request[60] =
      "GET url/se/index/ssss HTTP/1.1\r\n"
      "Host: localhost:8080\r";
  com::http::Parse parse(request);
  EXPECT_EQ(parse.Process(strlen(request)), -1);
  request[53] = '\n';
  request[54] = '\r';
  request[55] = '\n';
  EXPECT_EQ(parse.Process(3), 200);
}

TEST(Http, Parse_Content) {
  char request[100] =
      "GET url/se/index/ssss HTTP/1.1\r\n"
      "Host: localhost:8080\r\n"
      "Content-Length: 6\r\n";
  com::http::Parse parse(request);
  EXPECT_EQ(parse.Process(strlen(request)), -1);
  request[73] = '\r';
  EXPECT_EQ(parse.Process(1), -1);
  request[74] = '\n';
  EXPECT_EQ(parse.Process(1), -1);
  request[75] = '1';
  request[76] = '1';
  request[77] = '1';
  request[78] = '1';
  EXPECT_EQ(parse.Process(4), -1);
  request[79] = '1';
  request[80] = '1';
  EXPECT_EQ(parse.Process(2), 200);
}

TEST(Http, Parse_LineError) {
  char request[] = "121\n";
  com::http::Parse parse(request);
  EXPECT_EQ(parse.Process(strlen(request)), 400);
}

// HTTP文件请求响应
TEST(Http, FileResponse) {
  com::http::Packet packet;
  packet.Process("/test/test11/e.txt", com::http::Packet::FILE);
  writev(1, packet.file_buf(), 2);
}

// HTTP错误请求响应
TEST(Http, ErrorCodeResponse) {
  com::http::Packet packet;
  packet.Process(com::http::NOT_FOUND);
  char res[] =
      "HTTP/1.1 404 Not Found\r\n"
      "Content-Length: 105\r\n"
      "\r\n"
      "<head><title>404</title></head><body><center><h1>Not "
      "Found</h1></center><hr/><center>Meta</center></body>";
  std::string str_buf(packet.buf());
  std::string str_res(res);
  EXPECT_EQ(str_buf, str_res);
}

// HTTP数据请求响应
TEST(Http, DataResponse) {
  com::http::Packet packet;
  packet.Process("{\"Meta\":\"test\"}", com::http::Packet::DATA);

  char res[] =
      "HTTP/1.1 200 OK\r\n"
      "Content-Length: 15\r\n"
      "\r\n"
      "{\"Meta\":\"test\"}";
  std::string str_buf(packet.buf());
  std::string str_res(res);
  EXPECT_EQ(str_buf, str_res);
}

// 静态服务器路由表
// TEST(Router, StaticServer) {
//   EXPECT_EQ(com::Router::Instance()
//                 ->file_list("/test/test11/e.txt")
//                 .file_stat.st_size,
//             6);
// }

// 配置文件读取
TEST(Config, Readfile) {
  EXPECT_EQ(com::Config::Instance()->config().directory, "/home/www");
  EXPECT_EQ(com::Config::Instance()->config().port, 9006);
}

TEST(Logger, Output) { logger::Core::Instance(); }

TEST(Logger, Writer) {
  for (int i = 0; i < 60; ++i) {
    MINFO() << "INFO-"
            << "1111111"
            << "@@@@";
  }
  MDEBUG() << "DEBUG-"
           << "1111111";
  sleep(2);
  MWARN() << "WARN-"
          << "1111111";
  sleep(2);
  MERROR() << "ERROR-"
           << "1111111";
}

int main(int argc, char **argv) {
auto k = new meta::LogFormatter;
k->Init("%d{%Y %M %D %H %M AB} %r AAA %P ||%F|| AAABBB %L %m");
meta::Log::ptr log = std::make_shared<meta::Log>(meta::Log());
meta::LogEvent::ptr event = std::make_shared<meta::LogEvent>(meta::LogEvent());
std::cout<<k->pattern().size();
for (auto i : k->pattern())
{
  i->Format(std::cout,log,event);
}

#if META_LOOP
//  net::Core::Instance()->Start();
#else
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
#endif
}