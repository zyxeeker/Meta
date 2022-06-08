#include "pool.h"

namespace thread {

Pool::Pool(u_int32_t _cap, void (*fn)()) : m_capactity(_cap) {
  m_pool = new struct thread[m_capactity];
  auto ptr = m_pool;
  for (int i = 0; i < m_capactity; ++i) {
    *ptr = (struct thread){.id = i, .enty = fn};
    pthread_create(&(ptr->thread), nullptr, Wrapper, ptr);
    ++ptr;
  }
}

Pool::~Pool() { Join(); }

void* Pool::Wrapper(void* arg) {
  auto p = (struct thread*)arg;
  p->enty();
}

void Pool::Join() {
  auto ptr = m_pool;
  for (int i = 0; i < m_capactity; ++i) {
    pthread_join(ptr->thread, nullptr);
  }
}

}  // namespace thread