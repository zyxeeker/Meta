#include "timer_service.h"

namespace net {

TimerService* TimerService::inst = nullptr;

TimerService::TimerService() {
  m_lru = std::make_unique<algo::timer::TimerLru>(50, 50);
}

}  // namespace net