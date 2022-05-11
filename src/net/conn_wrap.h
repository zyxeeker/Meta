#ifndef SRC_NET_CONN_WRAP_H_
#define SRC_NET_CONN_WRAP_H_

#include <netinet/in.h>
#include <sys/socket.h>

#include <cstring>

namespace net {

class INetWrap {
 public:
  enum NetType { TCP, UDP };
  enum ConnResult { SUCCESS = 0, CREATE_FAILED, BIND_FAILED, LISTEN_FAILED };

  INetWrap(u_int32_t port) : m_port(port){};

  virtual ConnResult Init() = 0;
  virtual int get_fd() const = 0;

 protected:
  u_int32_t m_port = 9006;
};

INetWrap* CreateNetSokcet(INetWrap::NetType type, u_int32_t port);

// utils
static void InitSockAddress(sockaddr_in& addr, socklen_t len, u_int32_t& port) {
  bzero(&addr, len);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
}

}  // namespace net

#endif  // SRC_NET_CONN_WRAP_H_
