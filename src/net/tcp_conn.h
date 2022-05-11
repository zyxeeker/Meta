#ifndef SRC_NET_TCP_CONN_H_
#define SRC_NET_TCP_CONN_H_

#include "conn_wrap.h"

namespace net {

class TcpConn : public INetWrap {
 public:
  TcpConn(u_int32_t port) : INetWrap(port){};
  INetWrap::ConnResult Init() override;
  int get_fd() const override { return m_listen_fd; };

 private:
  int m_listen_fd;
};

}  // namespace net

#endif  // SRC_NET_TCP_CONN_H_
