#include "DefineSystem.h"

#ifdef WINDOWS

#include "WindowsSocket.h"

#define SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_USELESS_DEFINE  // define to tell
#undef SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_USELESS_DEFINE   // clang-format to
                                                              // not shuffle
                                                              // includes

#include <winsock2.h>  // NOLINT(llvm-include-order) cause of "Please include winsock2.h before windows.h"

#define SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_USELESS_DEFINE  // same
#undef SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_USELESS_DEFINE   //

#include <ws2tcpip.h>

#include <memory>
#include <string>
#include <vector>

namespace simple_http_server {

WindowsSocket::WindowsSocket(const std::string& address, int port) {
  wsaLock_.lock();

  if (!wsaStarted_) {
    WSAStartup(MAKEWORD(2, 2), &wsaData_);
    wsaStarted_ = true;
  }

  wsaLock_.unlock();

  CreateAddress(address, port, address_);

  socketDescriptor_ =
      socket(address_->ai_family, address_->ai_socktype, address_->ai_protocol);
}

WindowsSocket::~WindowsSocket() {
  closesocket(socketDescriptor_);
  freeaddrinfo(result);
  WSACleanup();
}

auto WindowsSocket::BindAndListen() -> bool {
  if (bind(socketDescriptor_, address_->ai_addr, address_->ai_addrlen) ==
      SOCKET_ERROR) {
    return false;
  }

  static constexpr int backlog = 5;
  return (listen(socketDescriptor_, backlog) != SOCKET_ERROR);
}

auto WindowsSocket::Connect() -> bool {
  return (connect(socketDescriptor_, address_->ai_addr, address_->ai_addrlen) ==
          0);
}

auto WindowsSocket::SendMessage(
    const std::unique_ptr<std::vector<WindowsSocket::Byte>>& message) -> bool {
  return (send(socketDescriptor_,
               reinterpret_cast<const char*>(message->data()),
               static_cast<int>(message->size()), 0) != SOCKET_ERROR);
}

auto WindowsSocket::SendMessage(
    const std::unique_ptr<std::vector<WindowsSocket::Byte>>& message,
    SocketDescriptor socket_addr) -> bool {
  return (send(socket_addr, reinterpret_cast<const char*>(message->data()),
               static_cast<int>(message->size()), 0) != -1);
}

auto WindowsSocket::SendMessageAndCloseClient(
    const std::unique_ptr<std::vector<WindowsSocket::Byte>>& message,
    SocketDescriptor socket_addr) -> bool {
  auto res = send(socket_addr, reinterpret_cast<const char*>(message->data()),
                  static_cast<int>(message->size()), 0) != -1;
  shutdown(socket_addr, SD_SEND);
  return res;
}

auto WindowsSocket::Accept() -> WindowsSocket::SocketDescriptor {
  auto client_addr = accept(socketDescriptor_, nullptr, nullptr);
  if (client_addr == INVALID_SOCKET) {
    throw std::runtime_error("could not accept socket");
  }

  return static_cast<int>(client_addr);
}

auto WindowsSocket::ReceiveMessage(SocketDescriptor client_socket)
    -> std::unique_ptr<std::vector<Byte>> {
  auto buffer =
      std::make_unique<std::vector<Byte>>(std::vector<Byte>(MAX_BUFFER_SIZE));

  const auto& len = recv(client_socket, reinterpret_cast<char*>(buffer->data()),
                         static_cast<int>(buffer->size()), 0);
  buffer->resize(len);

  return buffer;
}

auto WindowsSocket::ReceiveMessage() -> std::unique_ptr<std::vector<Byte>> {
  return ReceiveMessage(static_cast<int>(socketDescriptor_));
}

auto WindowsSocket::CreateAddress(const std::string& address, int port,
                                  decltype(address_)& addr_out) noexcept
    -> int {
  const addrinfo hints{.ai_flags = AI_PASSIVE,
                       .ai_family = AF_UNSPEC,
                       .ai_socktype = SOCK_STREAM,
                       .ai_protocol = IPPROTO_TCP,
                       .ai_addrlen = 0,
                       .ai_canonname = nullptr,
                       .ai_addr = nullptr,
                       .ai_next = nullptr};

  return getaddrinfo(address.c_str(), std::to_string(port).c_str(), &hints,
                     &addr_out);
}

}  // namespace simple_http_server

#endif  // WINDOWS
