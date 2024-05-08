#include "SocketFactory.h"

#include "util.h"

#ifdef POSIX
  #include "PosixSocket.h"
#elif defined WINDOWS
  #include "WindowsSocket.h"
#else
  #error Unknown platform
#endif

namespace simple_http_server {

std::unique_ptr<ISocket> SocketFactory::CreateSocket() {
#ifdef POSIX
  return std::make_unique<PosixSocket>();
#elif defined WINDOWS
  return std::make_unique<WindowsSocket>();
#else
  #error Unknown platform
#endif
}

} // simple_http_server
