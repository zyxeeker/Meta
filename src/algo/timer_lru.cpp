#include "timer_lru.h"

namespace algo {
namespace timer {

int TimerLru::Put(u_int32_t ip, ClientData client) {
  auto it = m_table.find(ip);

  if (it != m_table.end()) {
    it->second->second = client;
    m_list.splice(m_list.begin(), m_list, it->second);
    it->second = m_list.begin();
    return ADD_CLIENT_SUCCESS;
  }

  if (m_list.size() == m_capacity) {
    timeval tv;
    gettimeofday(&tv, 0);
    if (JudgeClient(tv, m_list.rbegin()->second.time) == IN_TIME)
      return CLIENT_LIST_FULL;
    else {
      m_table.erase(m_list.rbegin()->first);
      m_list.pop_back();
    }
  }

  m_list.emplace_front(std::pair<u_int32_t, ClientData>{ip, client});
  m_table[ip] = m_list.begin();
  return ADD_CLIENT_SUCCESS;
}

void TimerLru::Adjust() {
  timeval tv;
  gettimeofday(&tv, 0);

  for (auto i = m_list.rbegin(); i != m_list.rend(); ++i) {
    if (JudgeClient(tv, i->second.time) == OVER_TIME) {
      m_table.erase(i->first);
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