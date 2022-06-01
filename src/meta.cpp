#include "logger/core.h"
#include "net/core.h"

int main() {
  net::Core::Instance()->Start();

  logger::Core::Instance()->Stop();
  net::Core::Instance()->Stop();
  pthread_exit(nullptr);
}