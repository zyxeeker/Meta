#ifndef SRC_NET_UDP_CONN_H_
#define SRC_NET_UDP_CONN_H_

#include "conn_wrap.h"

namespace net {

class UdpConn : public INetWrap {
 public:
  UdpConn(u_int32_t port) : INetWrap(port){};
  INetWrap::ConnResult Init() override;
  int get_fd() const override { return m_socket_fd; }

 private:
  int m_socket_fd;
};

}  // namespace net

#endif  // SRC_NET_UDP_CONN_H_
