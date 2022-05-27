#include "writer.h"

#include <cstring>
#include <fstream>

namespace logger {

void Writer::Init(LogBuffer buffer, std::ofstream* file) {
  m_buffer = buffer;
  m_file = file;
}

void Writer::Process() {
  (*m_file) << m_buffer.date << " " << m_buffer.file_level_buf << " "
            << m_buffer.buffer << std::endl;
}

}  // namespace logger