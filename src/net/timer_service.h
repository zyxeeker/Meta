#ifndef SRC_NET_TIMER_SERVICE_H_
#define SRC_NET_TIMER_SERVICE_H_

#include <sys/socket.h>
#include <sys/time.h>

#include <functional>
#include <list>
#include <memory>
#include <unordered_map>

#include "algo/timer_lru.h"
#include "common/marco.h"

namespace net {

class TimerService {
 public:
  static TimerService* Instance() {
    if (!inst) inst = new TimerService();
    return inst;
  }

  algo::timer::ClientData Get(u_int32_t ip) { return m_lru->Get(ip); }

  int Put(u_int32_t ip, algo::timer::ClientData client) {
    return m_lru->Put(ip, client);
  }

  void Adjust() { m_lru->Adjust(); }

  void SetCapacity(u_int32_t cap) { m_lru->set_capacity(cap); }

  std::function<void(int32_t&)>& Callback() { return m_lru->callback(); }

 private:
  TimerService();
  static TimerService* inst;

  std::unique_ptr<algo::timer::TimerLru> m_lru;

  DISALLOW_COPY_AND_ASSIGN(TimerService);
};

}  // namespace net

#endif  // SRC_NET_TIMER_SERVICE_H_
