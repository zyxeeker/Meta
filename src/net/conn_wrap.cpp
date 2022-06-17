#include "conn_wrap.h"

#include <cstring>
#include <iostream>

#include "client_conn.h"
#include "logger/core.h"
#include "tcp_conn.h"
#include "udp_conn.h"

namespace net {

INetWrap* CreateNetSokcet(INetWrap::NetType type, u_int32_t port,
                          const char* addr) {
  switch (type) {
    case INetWrap::TCP: {
      auto* p = new TcpConn(port);
      if (p->Init() > 0) {
        delete p;
        return nullptr;
      }
      return p;
    }
    case INetWrap::UDP: {
      auto* p = new UdpConn(port);
      if (p->Init() > 0) {
        delete p;
        return nullptr;
      }
      return p;
    }
    case INetWrap::CLIENT: {
      auto* p = new ClientConn(addr, port);
      if (p->Init() > 0) {
        delete p;
        return nullptr;
      }
      return p;
    }
    default:
      return nullptr;
  }
}

}  // namespace net
