#include "timer_lru.h"

namespace algo {
namespace timer {

int TimerLru::Put(u_int32_t ip, ClientData client) {
  auto it = table().find(ip);

  if (it != table().end()) {
    it->second->second = client;
    list().splice(list().begin(), list(), it->second);
    it->second = list().begin();
    return ADD_CLIENT_SUCCESS;
  }

  if (list().size() == capacity()) {
    timeval tv;
    gettimeofday(&tv, 0);
    if (JudgeClient(tv, list().rbegin()->second.time) == IN_TIME)
      return CLIENT_LIST_FULL;
    else {
      table().erase(list().rbegin()->first);
      list().pop_back();
    }
  }

  list().emplace_front(std::pair<u_int32_t, ClientData>{ip, client});
  table()[ip] = list().begin();
  return ADD_CLIENT_SUCCESS;
}

void TimerLru::Adjust() {
  timeval tv;
  gettimeofday(&tv, 0);

  for (auto i = list().rbegin(); i != list().rend(); ++i) {
    if (JudgeClient(tv, i->second.time) == OVER_TIME) {
      table().erase(i->first);
      m_callback(i->second.fd);
    }
  }
}

TimerLru::ClientState TimerLru::JudgeClient(const timeval& tv,
                                            const time_t& time) {
  if (tv.tv_sec - time >= m_delay) return OVER_TIME;
  return IN_TIME;
}

}  // namespace timer
}  // namespace algo