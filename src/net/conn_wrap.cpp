#include "conn_wrap.h"

#include <cstring>
#include <iostream>

#include "logger/core.h"
#include "tcp_conn.h"
#include "udp_conn.h"

namespace net {

INetWrap* CreateNetSokcet(INetWrap::NetType type, u_int32_t port) {
  switch (type) {
    case INetWrap::NetType::TCP: {
      auto* p = new TcpConn(port);
      if (p->Init() > 0) {
        MERROR() << "TCP: Port is obtained!";
        delete p;
        return nullptr;
      }
      return p;
    }
    case INetWrap::NetType::UDP: {
      auto* p = new UdpConn(port);
      if (p->Init() > 0) {
        MERROR() << "UDP: Port is obtained!";
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
