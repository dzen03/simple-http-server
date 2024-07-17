#ifndef SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_
#define SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_

#include "DefineSystem.h"

#ifdef WINDOWS

#include "ISocket.h"

#include <memory>
#include <string>
#include <vector>
#include <winsock2.h> // NOLINT(llvm-include-order) cause of "Please include winsock2.h before windows.h"
#include <windows.h>
#include <ws2tcpip.h>

namespace simple_http_server {

class WindowsSocket : public ISocket {
 public:
  WindowsSocket();
  ~WindowsSocket() override;

  auto BindAndListen(const std::string& address, int port) -> bool override;
  auto Connect(std::string address, int port) -> bool override;

#undef SendMessage // cause of mingw

  auto SendMessage(const std::unique_ptr<std::vector<Byte>> &message) -> bool override;
  auto SendMessage(const std::unique_ptr<std::vector<Byte>> &message,
                   SocketDescriptor socket_addr) -> bool override;
  auto SendMessageAndCloseClient(const std::unique_ptr<std::vector<Byte>> &message,
                                 SocketDescriptor socket_addr) -> bool override;
  auto Accept() -> SocketDescriptor override;

  auto ReceiveMessage() -> std::unique_ptr<std::vector<Byte>> override;
  auto ReceiveMessage(SocketDescriptor client_socket) -> std::unique_ptr<std::vector<Byte>> override;
 private:
  SOCKET socketDescriptor_;
  addrinfo address_;

  inline static WSADATA wsaData_ {};
  inline static bool wsaStarted_ = false;
  inline static std::mutex wsaLock_ {};

  static auto CreateAddress(const std::string &address, int port) -> addrinfo;
};

} // namespace simple_http_server

#endif // WINDOWS

#endif //SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_
