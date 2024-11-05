#ifndef SIMPLE_HTTP_SERVER_LIBS_WINDOWSSOCKET_H_
#define SIMPLE_HTTP_SERVER_LIBS_WINDOWSSOCKET_H_

#include "DefineSystem.h"

#ifdef WINDOWS

#include "ISocket.h"

#define SIMPLE_HTTP_SERVER_LIBS_WINDOWSSOCKET_H_USELESS_DEFINE
#undef SIMPLE_HTTP_SERVER_LIBS_WINDOWSSOCKET_H_USELESS_DEFINE
// defined to tell clang-format to not move includes around

#include <winsock2.h>  // NOLINT(llvm-include-order) cause of "Please include winsock2.h before windows.h"

#define SIMPLE_HTTP_SERVER_LIBS_WINDOWSSOCKET_H_USELESS_DEFINE  // same
#undef SIMPLE_HTTP_SERVER_LIBS_WINDOWSSOCKET_H_USELESS_DEFINE   //

#include <windows.h>
#include <ws2tcpip.h>

#include <memory>
#include <string>
#include <vector>

namespace simple_http_server {

class WindowsSocket : public ISocket {
 public:
  WindowsSocket(const std::string& address, int port);
  ~WindowsSocket() override;

  WindowsSocket(const WindowsSocket& source) = default;
  WindowsSocket(WindowsSocket&& source) = default;

  auto operator=(const WindowsSocket& source) -> WindowsSocket& = default;
  auto operator=(WindowsSocket&& source) -> WindowsSocket& = default;

  auto BindAndListen() -> bool override;
  auto Connect() -> bool override;

#undef SendMessage  // cause of mingw

  auto SendMessage(const std::unique_ptr<std::vector<Byte>>& message)
      -> bool override;
  auto SendMessage(const std::unique_ptr<std::vector<Byte>>& message,
                   SocketDescriptor socket_addr) -> bool override;
  auto SendMessageAndCloseClient(
      const std::unique_ptr<std::vector<Byte>>& message,
      SocketDescriptor socket_addr) -> bool override;
  auto Accept() -> SocketDescriptor override;

  auto ReceiveMessage() -> std::unique_ptr<std::vector<Byte>> override;
  auto ReceiveMessage(SocketDescriptor client_socket)
      -> std::unique_ptr<std::vector<Byte>> override;

 private:
  SOCKET socketDescriptor_;
  addrinfo* address_ = nullptr;

  inline static WSADATA wsaData_{};
  inline static bool wsaStarted_ = false;
  inline static std::mutex wsaLock_{};

  static auto CreateAddress(const std::string& address, int port,
                            decltype(address_)& addr_out) noexcept -> int;
};

}  // namespace simple_http_server

#endif  // WINDOWS

#endif  // SIMPLE_HTTP_SERVER_LIBS_WINDOWSSOCKET_H_
