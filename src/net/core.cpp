#include "core.h"

#include <arpa/inet.h>

#include "logger/core.h"

#ifdef META_DEBUG
#define _DEBUG 1
#else
#define _DEBUG 0
#endif

namespace net {

Core* Core::inst = nullptr;

Core::Core() {
  m_epoll.reset(new com::Epoll(10000));
  m_reader_pool.reset(new thread::Pool(50, 50));
  m_writer_pool.reset(new thread::Pool(50, 50));
  m_process_pool.reset(new thread::Pool(50, 50));

  m_process_core = new worker::Core[65536];
  m_process_core->k_epoll = m_epoll;
}

void Core::Start() {
  auto p = CreateNetSokcet(INetWrap::NetType::TCP, 9006);
  if (p)
    SocketListCtl(p, LIST_CTL_ADD);
  else {
    MERROR() << "CREATE NET SERIVCE FAILED!";
    return;
  }
// 端口复用 IN DEBUG
#if _DEBUG
  int reuse = 1;
  setsockopt(p->get_fd(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif
  Loop();
}

void Core::Loop() {
  MINFO() << "MAIN LOOP START!";
  while (1) {
    auto total = m_epoll->Wait();

    /* Some interfaces are never restarted after being interrupted by a signal
     * handler, regardless of the use of SA_RESTART; they always fail with the
     * error EINTR when interrupted by a signal handler */

    if (total == -1 && errno != EINTR) {
      MERROR() << "MAIN LOOP STOP: "
               << "EPOLL FAILRUE!";
      break;
    } else if (total == -1)
      continue;

    for (int i = 0; i < total; ++i) {
      auto socket = m_epoll->events(i)->data.fd;
      // 新客户端连接
      if (m_socket_list.find(socket) != m_socket_list.end()) {
        sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);
        auto confd = accept(socket, (sockaddr*)&client_addr, &length);
        MDEBUG() << "CLIENT IP: " << inet_ntoa(client_addr.sin_addr);
        // 初始化处理类
        m_process_core[confd].Init(confd);

        m_epoll->Add(confd, true);
      }
      // 管道数据
      else if (m_epoll->events(i)->events &
               (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
        Close(socket);
      }
      // 客户端数据可读
      else if (m_epoll->events(i)->events & EPOLLIN) {
        if (AddReaderTask(m_process_core[socket].reader())) continue;
        // 过载
        m_process_core[socket].CreateServiceUnavailableResp();
        m_epoll->Mod(socket, EPOLLOUT);
      }
      // 客户端数据可写
      else if (m_epoll->events(i)->events & EPOLLOUT) {
        if (AddWriterTask(m_process_core[socket].writer())) continue;
        // 过载
        m_epoll->Mod(socket, EPOLLOUT);
      }
    }
  }
}

void Core::SocketListCtl(INetWrap* net, ListCtl ctl) {
  auto socket = net->get_fd();

  if (ctl == LIST_CTL_ADD) {
    m_socket_list[socket].reset(net);
    m_epoll->Add(socket, false);
  }
  if (ctl == LIST_CTL_REMOVE) {
    auto it = m_socket_list.find(socket);
    if (it != m_socket_list.end()) {
      m_socket_list.erase(it);
      Close(socket);
    }
  }
}

void Core::Close(int32_t& fd) {
  // 从epoll管理中移除并关闭
  m_epoll->Del(fd);
  close(fd);
}

// Signal operation
// int Core::AddSig(com::Signal* sig) {
//   if (sig->Init() == -1) return -1;

//   m_epoll->Add(m_timer->read_fd());

//   return 0;
// }

}  // namespace net