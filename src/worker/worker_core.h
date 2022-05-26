#ifndef SRC_WORKER_WORKER_CORE_H_
#define SRC_WORKER_WORKER_CORE_H_

#include <memory>

#include "thread/task.h"
#include "worker.h"
#include "worker_io.h"

namespace worker {

class Core : public thread::Task {
 public:
  Core();

  void Init(int confd);

  void Process() override;

  // 重置数据成员
  void Reset();
  // 将自己放入线程池中
  bool ToPool();
  // 过载时的应答
  void CreateServiceUnavailableResp() {
    CreateErrorPacket(com::http::SERVICE_UNAVAILABLE);
  }

  Info* info() { return m_info.get(); }
  ReaderObject* read_object() { return m_reader_object.get(); }
  WriterObject* write_object() { return m_writer_object.get(); }
  io::Reader* reader() { return m_reader.get(); }
  io::Writer* writer() { return m_writer.get(); }

 private:
  // 内部初始化
  void Init();
  // 回应错误代码
  inline void CreateErrorPacket(com::http::HttpCode code) {
    m_info->packet_handler->Process(code);
  }
  // 回应文件
  inline void CreateFilePacket(std::string file_path) {
    m_info->packet_handler->Process(file_path, com::http::Packet::FILE);
  }
  // 回应数据
  inline void CreateDataPacket(std::string data) {
    m_info->packet_handler->Process(data, com::http::Packet::DATA);
  }

 public:
  static std::shared_ptr<com::Epoll> k_epoll;

 private:
  std::shared_ptr<Info> m_info;
  std::shared_ptr<io::Reader> m_reader;
  std::shared_ptr<io::Writer> m_writer;
  std::shared_ptr<ReaderObject> m_reader_object;
  std::shared_ptr<WriterObject> m_writer_object;
  // Info* m_info = nullptr;
  // io::Reader* m_reader = nullptr;
  // io::Writer* m_writer = nullptr;
  // ReaderObject* m_reader_object = nullptr;
  // WriterObject* m_writer_object = nullptr;
};

}  // namespace worker

#endif  // SRC_WORKER_WORKER_CORE_H_
