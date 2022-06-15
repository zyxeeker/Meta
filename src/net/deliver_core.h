#ifndef SRC_NET_DELIVER_CORE_H_
#define SRC_NET_DELIVER_CORE_H_

#include <arpa/inet.h>
#include <sys/socket.h>

#include "net/object.h"
#include "thread/pool.h"

namespace net {

class DeliverCore {
 public:
  DeliverCore(const char* addr, int port, Object* obj);

  void Init();
  void Close();
  void Process();

 private:
  int m_socket = -1;
  struct sockaddr_in m_addr;
  int m_epoll;
  Object* m_obj = nullptr;
  bool m_run = true;
};

}  // namespace net

#endif  // SRC_NET_DELIVER_CORE_H_
