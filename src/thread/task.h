#ifndef SRC_THREAD_TASK_H_
#define SRC_THREAD_TASK_H_

namespace thread {

class Task {
 public:
  virtual void Process() = 0;
};

}  // namespace thread

#endif  // SRC_THREAD_TASK_H_
