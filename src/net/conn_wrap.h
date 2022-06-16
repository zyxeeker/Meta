#ifndef SRC_NET_CONN_WRAP_H_
#define SRC_NET_CONN_WRAP_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cstring>

namespace net {

class INetWrap {
 public:
  enum NetType { TCP, UDP, CLIENT };
  enum ConnResult {
    SUCCESS = 0,
    CREATE_FAILED,
    BIND_FAILED,
    LISTEN_FAILED,
    CONN_EXISTED,
    CONNECT_FAILED
  };

  INetWrap(u_int32_t port) : m_port(port){};
  INetWrap(const char* addr, u_int32_t port)
      : m_addr(inet_addr(addr)), m_port(port){};

  virtual ConnResult Init() = 0;
  virtual void Close(){};
  virtual int get_fd() const = 0;

 protected:
  in_addr_t m_addr = INADDR_ANY;
  u_int32_t m_port;
};

INetWrap* CreateNetSokcet(INetWrap::NetType type, u_int32_t port,
                          const char* addr = nullptr);

// utils
static void InitSockAddress(sockaddr_in& sock_addr, socklen_t len,
                            in_addr_t addr, u_int32_t& port) {
  bzero(&sock_addr, len);
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = htons(port);
  sock_addr.sin_addr.s_addr = addr;
}

}  // namespace net

#endif  // SRC_NET_CONN_WRAP_H_
