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
#include "thread/pool.h"
#include "worker/worker_core.h"

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

  bool AddReaderTask(thread::Task *task) {
    if (m_reader_pool->Append(task) != thread::Pool::ADD_REQUEST_SUCCESS)
      return false;
    return true;
  }
  bool AddWriterTask(thread::Task *task) {
    if (m_writer_pool->Append(task) != thread::Pool::ADD_REQUEST_SUCCESS)
      return false;
    return true;
  }
  bool AddProcessTask(thread::Task *task) {
    if (m_process_pool->Append(task) != thread::Pool::ADD_REQUEST_SUCCESS)
      return false;
    return true;
  }

 private:
  Core();
  void Loop();

 private:
  static Core *inst;

  worker::Core *m_process_core = nullptr;

  // epoll
  std::shared_ptr<com::Epoll> m_epoll;
  std::unique_ptr<thread::Pool> m_reader_pool;
  std::unique_ptr<thread::Pool> m_writer_pool;
  std::unique_ptr<thread::Pool> m_process_pool;

  // 多个端口服务
  std::unordered_map<int, std::shared_ptr<INetWrap>> m_socket_list;

  DISALLOW_COPY_AND_ASSIGN(Core);
};

}  // namespace net

#endif  // SRC_NET_CORE_H_
