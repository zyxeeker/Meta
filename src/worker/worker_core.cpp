#include "worker_core.h"

#include <iostream>

#include "component/config.h"
#include "component/router.h"
#include "net/core.h"

namespace worker {

std::shared_ptr<com::Epoll> Core::k_epoll = nullptr;

Core::Core()
    : m_info(new Info),
      m_reader_object(new ReaderObject),
      m_writer_object(new WriterObject) {
  auto size = com::Config::Instance()->config().read_buf_length;
  m_reader_object->readed_buf = new char[size];

  m_reader_object->to_pool_cb = std::bind(&Core::ToPool, this);
  m_writer_object->reset_cb = std::bind(&Core::Reset, this);

  m_reader.reset(new io::Reader(m_info.get(), m_reader_object.get()));
  m_writer.reset(new io::Writer(m_info.get(), m_writer_object.get()));
}

void Core::Init(int confd) {
  m_info->fd = confd;
  m_info->epoll = k_epoll;

  Init();
}

void Core::Process() {
  if (m_info->http_code != 200) {
    CreateErrorPacket(m_info->http_code);
  } else {
    auto path = m_info->parse_handler->url();
    auto version = m_info->parse_handler->version();

    auto find_res = com::Router::Instance()->Find(path);

    if (!find_res)
      CreateErrorPacket(com::http::NOT_FOUND);
    else
      CreateFilePacket(path);
  }
  k_epoll->Mod(m_info->fd, EPOLLOUT);
}

void Core::Reset() { Init(); }

bool Core::ToPool() {
  if (net::Core::Instance()->AddProcessTask(this) ==
      thread::Pool::ADD_REQUEST_SUCCESS)
    return true;
  return false;
}

// 内部初始化
void Core::Init() {
  bzero(m_reader_object->readed_buf,
        com::Config::Instance()->config().read_buf_length);

  m_reader_object->readed_offset = 0;
  m_reader_object->prev_reaed_offset = 0;
  m_writer_object->write_offset = 0;
  m_info->parse_handler.reset(
      new com::http::Parse(m_reader_object->readed_buf));
  m_info->packet_handler.reset(new com::http::Packet);
}

}  // namespace worker