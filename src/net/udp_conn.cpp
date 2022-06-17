#include "udp_conn.h"

#include "logger/core.h"

namespace net {

INetWrap::ConnResult UdpConn::Init() {
  sockaddr_in address;
  InitSockAddress(address, sizeof(address), m_addr, m_port);

  m_socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
  if (m_socket_fd < 0) {
    MERROR() << "Failed to create socket: " << strerror(errno);
    return CREATE_FAILED;
  }

  int result = bind(m_socket_fd, (sockaddr*)&address, sizeof(address));
  if (result < 0) {
    MERROR() << "Failed to bind: " << strerror(errno);
    return BIND_FAILED;
  }

  return SUCCESS;
}

}  // namespace net