#ifndef SRC_EVENT_WORKER_IO_H_
#define SRC_EVENT_WORKER_IO_H_

#include <sys/uio.h>

#include <cstdio>
#include <memory>

#include "thread/task.h"
#include "worker.h"

namespace worker {
namespace io {

class Reader : public thread::Task {
 public:
  Reader(Info* _info, ReaderObject* _object)
      : m_info(_info), m_object(_object) {}

  void Process() override;

 private:
  Info* m_info = nullptr;
  ReaderObject* m_object = nullptr;
};

class Writer : public thread::Task {
 public:
  Writer(Info* _info, WriterObject* _object)
      : m_info(_info), m_object(_object) {}

  void Process() override;

 private:
  Info* m_info = nullptr;
  WriterObject* m_object = nullptr;
};

}  // namespace io
}  // namespace worker

#endif  // SRC_EVENT_WORKER_IO_H_
