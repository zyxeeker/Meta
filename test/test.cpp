#include <iostream>

#include "component/config.h"
#include "component/http_packet.h"
#include "component/http_parse.h"
#include "component/router.h"
#include "gtest/gtest.h"
#include "net/service.h"
#include "thread/pool.h"

TEST(Net, MainService) { net::Service::Instance()->Start(); }

// HTTP解析
TEST(Http, Parse) {
  char request[] =
      "GET url/se/index/ssss HTTP/1.1\r\n"
      "Host: localhost:8080\r\n"
      "Content-Length: 6\r\n"
      "\r\n"
      "asasaa";
  com::http::Parse parse(request, strlen(request));
  parse.Process();
  EXPECT_EQ(parse.method(), com::http::Method::GET);
  EXPECT_EQ(parse.url(), "url/se/index/ssss");
  EXPECT_EQ(parse.version(), "HTTP/1.1");
  EXPECT_EQ(parse.header().at("Host"), "localhost:8080");
  EXPECT_EQ(parse.header().at("Content-Length"), "6");
  EXPECT_EQ(parse.content_length(), 6);
  EXPECT_EQ(parse.content(), "asasaa");
}

// HTTP文件请求响应
TEST(Http, FileResponse) {
  com::http::Packet packet("HTTP/1.1", "/test/test11/e.txt",
                           com::http::Packet::FILE);
  packet.Process();
  char res[] =
      "HTTP/1.1 200 OK\r\n"
      "\r\n"
      "TTTTT\n";
  EXPECT_EQ(packet.buf(), res);
  // std::cout << packet.buf() << std::endl;
}

// HTTP错误请求响应
TEST(Http, ErrorCodeResponse) {
  com::http::Packet packet("HTTP/1.1", com::http::HTTP_CODE::BAD_REQUEST);
  packet.Process();
  char res[] =
      "HTTP/1.1 400 Bad Request\r\n"
      "\r\n"
      "<head><title>400</title></head><body><center><h1>Bad "
      "Request</h1></center><hr/><center>Meta</center></body>";
  EXPECT_EQ(packet.buf(), res);
  // std::cout << packet.buf() << std::endl;
}

// HTTP数据请求响应
TEST(Http, DataResponse) {
  com::http::Packet packet("HTTP/1.1", "{\"Meta\":\"test\"}",
                           com::http::Packet::DATA);
  packet.Process();
  char res[] =
      "HTTP/1.1 200 OK\r\n"
      "\r\n"
      "{\"Meta\":\"test\"}";
  EXPECT_EQ(packet.buf(), res);
}

// 静态服务器路由表
TEST(Router, StaticServer) {
  EXPECT_EQ(com::Router::Instance()
                ->file_list("/test/test11/e.txt")
                .file_stat.st_size,
            6);
}

// 配置文件读取
TEST(Config, Readfile) {
  EXPECT_EQ(com::Config::Instance()->config().directory, "/home/www");
  EXPECT_EQ(com::Config::Instance()->config().port, 9006);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}