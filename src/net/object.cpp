#include "object.h"

#include "component/config.h"
#include "logger/core.h"

namespace net {

int Object::epoll_fd;

Object::Object() {
  auto size = com::Config::Instance()->config().read_buf_length;
  readed_buf = new char[size];
}

Object::~Object() {
  delete[] readed_buf;
  delete[] write_buf;
}

void Object::Init() {
  // MDEBUG() << "INIT!";
  bzero(readed_buf, com::Config::Instance()->config().read_buf_length);
  prev_reaed_offset = 0;
  readed_offset = 0;
  write_offset = 0;
  parse_handler.reset(new com::http::Parse(readed_buf));
  packet_handler.reset(new com::http::Packet);
  // while (chunk_data.size() > 0) {
  //   auto i = chunk_data.front();
  //   chunk_data.pop_front();
  //   delete i;
  // }
}

bool Object::InitProxy(const char* addr, int port) {
  if (client != nullptr && client->get_fd() != -1) {
    if (client->get_fd() != -1)
      return true;
    else if (client->Init() > 0) {
      client.reset();
      return false;
    }
  }

  auto p = CreateNetSokcet(INetWrap::CLIENT, port, addr);
  if (!p) return false;

  client.reset(p);

  packet_handler->SetTrans();
  return true;
}

void Object::Close() {
  if (client != nullptr) client->Close();
}

void Object::EpollMod(int op) { com::EpollModFd(epoll_fd, fd, op); }

}  // namespace net