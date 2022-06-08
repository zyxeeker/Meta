#ifndef SRC_NET_CORE_H_
#define SRC_NET_CORE_H_

#include <sys/epoll.h>
#include <unistd.h>

#include <memory>
#include <unordered_map>

#include "common/marco.h"
#include "component/epoll.h"
#include "component/signal.h"
#include "conn_wrap.h"
#include "object.h"
#include "thread/pool.h"

namespace net {

class Core {
 public:
  enum ListCtl { LIST_CTL_ADD, LIST_CTL_REMOVE };

  static Core *Instance() {
    if (!inst) inst = new Core;
    return inst;
  }

  void Start();
  // socket列表操作 保存多个监听端口
  void SocketListCtl(INetWrap *net, ListCtl ctl);
  // 关闭fd
  void Close(int32_t &fd);

 private:
  Core();
  void Loop();

 private:
  static Core *inst;

  Object *m_object = nullptr;

  // epoll
  std::shared_ptr<com::Epoll> m_epoll;

  // 多个端口服务
  std::unordered_map<int, std::shared_ptr<INetWrap>> m_socket_list;

  DISALLOW_COPY_AND_ASSIGN(Core);
};

}  // namespace net

#endif  // SRC_NET_CORE_H_
