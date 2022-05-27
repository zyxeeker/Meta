#ifndef SRC_LOGGER_WRITER_H_
#define SRC_LOGGER_WRITER_H_

#include <iostream>
#include <string>

#include "log.h"
#include "thread/task.h"

namespace logger {

class Writer : public thread::Task {
 public:
  void Init(LogBuffer buffer, std::ofstream* file);
  void Process() override;

 private:
  LogBuffer m_buffer;
  std::ofstream* m_file = nullptr;
};

}  // namespace logger

#endif  // SRC_LOGGER_WRITER_H_
