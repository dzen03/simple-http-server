#include "SocketFactory.h"

#include <memory>
#include <string>

#include "DefineSystem.h"
#include "ISocket.h"

#ifdef POSIX
#include "PosixSocket.h"
#elif defined WINDOWS
#include "WindowsSocket.h"
#else
#error Unknown platform
#endif

namespace simple_http_server {

auto SocketFactory::CreateSocket(const std::string& address,
                                 int port) -> std::unique_ptr<ISocket> {
#ifdef POSIX
  return std::make_unique<PosixSocket>(address, port);
#elif defined WINDOWS
  return std::make_unique<WindowsSocket>(address, port);
#else
#error Unknown platform
#endif
}

}  // namespace simple_http_server
