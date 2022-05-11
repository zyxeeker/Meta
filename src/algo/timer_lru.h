#ifndef SRC_ALGO_TIMER_LRU_H_
#define SRC_ALGO_TIMER_LRU_H_

#include <sys/time.h>

#include <functional>

#include "lru.h"

namespace algo {
namespace timer {

typedef struct client_data_ {
  int32_t fd;
  time_t time;  // 新客户端到达和已连接客户端数据到达时将会刷新时间
} ClientData;

class TimerLru : public Lru<u_int32_t, ClientData> {
 public:
  enum ClientState { OVER_TIME, IN_TIME };
  enum Result { ADD_CLIENT_SUCCESS = 0, CLIENT_LIST_FULL };

  TimerLru(time_t _delay, size_t _cap)
      : m_delay(_delay), Lru<u_int32_t, ClientData>(_cap){};

  int Put(u_int32_t ip, ClientData client) override;

  // 删除过期的客户端
  void Adjust();

  std::function<void(int32_t&)>& callback() { return m_callback; }

 private:
  // 判断客户端是否过期
  inline ClientState JudgeClient(const timeval& tv, const time_t& time);

 private:
  time_t m_delay;
  // 回调函数
  std::function<void(int32_t&)> m_callback;
};

}  // namespace timer
}  // namespace algo

#endif  // SRC_ALGO_TIMER_LRU_H_
