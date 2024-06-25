#include "DefineSystem.h"
#include "ISocket.h"
#include "SocketFactory.h"

#include <memory>

#ifdef POSIX
  #include "PosixSocket.h"
#elif defined WINDOWS
  #include "WindowsSocket.h"
#else
  #error Unknown platform
#endif

namespace simple_http_server {

auto SocketFactory::CreateSocket() -> std::unique_ptr<ISocket> {
#ifdef POSIX
  return std::make_unique<PosixSocket>();
#elif defined WINDOWS
  return std::make_unique<WindowsSocket>();
#else
  #error Unknown platform
#endif
}

} // namespace simple_http_server
