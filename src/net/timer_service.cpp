#include "timer_service.h"

namespace net {

TimerService* TimerService::inst = nullptr;

TimerService::TimerService() { m_lru.reset(new algo::timer::TimerLru(50, 50)); }

}  // namespace net