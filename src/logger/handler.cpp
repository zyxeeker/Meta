#include "handler.h"

#include "core.h"

#ifdef META_DEBUG
#define _DEBUG 1
#else
#define _DEBUG 0
#endif

namespace logger {

Handler::~Handler() {
  WriteToConsole();
  Core::Instance()->AppendTask(m_buffer);
}

void Handler::GetTime() {
  time_t now = time(nullptr);
  strftime(m_buffer.date, 50, "%Y_%m_%d_%H:%M:%S", localtime(&now));
}

void Handler::TransLevel(LogLevel level) {
  switch (level) {
    case DEBUG:
#if _DEBUG
      WriteToBuffer("DEBUG", k_magenta);
#endif
      break;
    case INFO:
      WriteToBuffer("INFO", k_cyan);
      break;
    case WARN:
      WriteToBuffer("WARN", k_yellow);
      break;
    case ERROR:
      WriteToBuffer("ERROR", k_red);
      break;
  }
}

void Handler::WriteToBuffer(const char* level, const char* color) {
  sprintf(m_buffer.level_buf, "[%s%s%s]:", color, level, k_color_end);
  sprintf(m_buffer.file_level_buf, "[%s]:", level);
}

void Handler::WriteToConsole() {
  printf("%s %s %s\n", m_buffer.date, m_buffer.level_buf,
         m_buffer.buffer.c_str());
}

}  // namespace logger