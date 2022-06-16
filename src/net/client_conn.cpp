#include "client_conn.h"

#include <unistd.h>

#include <cstdio>

#include "common/util.h"

namespace net {

INetWrap::ConnResult ClientConn::Init() {
  if (m_client_fd != -1) return CONN_EXISTED;

  m_client_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address;
  InitSockAddress(address, sizeof(address), m_addr, m_port);

  int res = connect(m_client_fd, (sockaddr*)&address, sizeof(address));

  printf("SOCK: %d\n", m_client_fd);
  if (res < 0) {
    printf("RES: %d\n", res);
    return CONNECT_FAILED;
  }
  // set NOBLOCK
  util::SetNoBlock(m_client_fd);
  return SUCCESS;
}

void ClientConn::Close() {
  close(m_client_fd);
  m_client_fd = -1;
}

}  // namespace net