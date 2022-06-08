#ifndef SRC_THREAD_ABSTRACT_WAITER_H_
#define SRC_THREAD_ABSTRACT_WAITER_H_

#include "thread.h"

namespace thread {

class AbstractWaiter {
 public:
  virtual Job* GetObject(int type) = 0;
};

}  // namespace thread

#endif  // SRC_THREAD_ABSTRACT_WAITER_H_
