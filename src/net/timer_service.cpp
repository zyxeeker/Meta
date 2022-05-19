#include "timer_service.h"

#include "component/config.h"

namespace net {

TimerService* TimerService::inst = nullptr;

TimerService::TimerService() {
  m_lru.reset(new algo::timer::TimerLru(
      com::Config::Instance()->config().client_alive_time,
      com::Config::Instance()->config().client_capacity));
}

}  // namespace net