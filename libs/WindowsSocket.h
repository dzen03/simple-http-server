#ifndef SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_
#define SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_

#include "DefineSystem.h"

#ifdef WINDOWS

#include "ISocket.h"

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <memory>
#include <string>
#include <vector>

namespace simple_http_server {

class WindowsSocket : public ISocket {
 public:
  WindowsSocket();
  ~WindowsSocket() override;

  bool BindAndListen(const std::string& address, int port) override;
  bool Connect(std::string address, int port) override;

#undef SendMessage // cause of mingw

  bool SendMessage(const std::unique_ptr<std::vector<Byte>> &message) override;
  bool SendMessage(const std::unique_ptr<std::vector<Byte>> &message,
                   SocketDescriptor socket_addr) override;
  bool SendMessageAndCloseClient(const std::unique_ptr<std::vector<Byte>> &message,
                                 SocketDescriptor socket_addr) override;
  SocketDescriptor Accept() override;

  auto ReceiveMessage() -> std::unique_ptr<std::vector<Byte>> override;
  auto ReceiveMessage(SocketDescriptor client_socket) -> std::unique_ptr<std::vector<Byte>> override;
 private:
  SOCKET socketDescriptor_;
  addrinfo address_;

  inline static WSADATA wsaData_ {};
  inline static bool wsaStarted_ = false;
  inline static std::mutex wsaLock_ {};

  static addrinfo CreateAddress(const std::string &address, int port);
};

} // SimpleHttpServer

#endif // WINDOWS

#endif //SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_
