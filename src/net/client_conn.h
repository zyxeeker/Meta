#ifndef SRC_NET_CLIENT_CONN_H_
#define SRC_NET_CLIENT_CONN_H_

#include "conn_wrap.h"

namespace net {

class ClientConn : public INetWrap {
 public:
  ClientConn(const char* addr, u_int32_t port) : INetWrap(addr, port){};
  INetWrap::ConnResult Init() override;
  void Close() override;

  int get_fd() const override { return m_client_fd; };

 private:
  int m_client_fd = -1;
};

}  // namespace net

#endif  // SRC_NET_CLIENT_CONN_H_
