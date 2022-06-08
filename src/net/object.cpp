#include "object.h"

#include "component/config.h"

namespace net {

std::shared_ptr<com::Epoll> Object::epoll = nullptr;

Object::Object() {
  auto size = com::Config::Instance()->config().read_buf_length;
  readed_buf = new char[size];
}

Object::~Object() {
  delete[] readed_buf;
  delete[] write_buf;
}

void Object::Init() {
  bzero(readed_buf, com::Config::Instance()->config().read_buf_length);
  prev_reaed_offset = 0;
  readed_offset = 0;
  write_offset = 0;
  parse_handler.reset(new com::http::Parse(readed_buf));
  packet_handler.reset(new com::http::Packet);
}

}  // namespace net