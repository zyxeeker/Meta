#include "tcp_conn.h"

#include <netinet/in.h>
#include <sys/socket.h>

#include <cstring>
#include <iostream>

#include "logger/core.h"

namespace net {

INetWrap::ConnResult TcpConn::Init() {
  sockaddr_in address;
  InitSockAddress(address, sizeof(address), m_addr, m_port);

  m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_listen_fd < 0) {
    MERROR() << "Failed to create socket: " << strerror(errno);
    return CREATE_FAILED;
  }

  int result = bind(m_listen_fd, (sockaddr*)&address, sizeof(address));
  if (result < 0) {
    MERROR() << "Failed to bind: " << strerror(errno);
    return BIND_FAILED;
  }

  result = listen(m_listen_fd, 25);
  if (result < 0) {
    MERROR() << "Failed to listen: " << strerror(errno);
    return LISTEN_FAILED;
  }

  return SUCCESS;
}

}  // namespace net