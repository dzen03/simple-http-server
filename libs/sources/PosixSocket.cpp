#include "DefineSystem.h"

#ifdef POSIX

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <memory>
#include <string>
#include <vector>

#include "PosixSocket.h"

namespace simple_http_server {

PosixSocket::PosixSocket(const std::string& address, int port) {
  CreateAddress(address, port, address_);

  socketDescriptor_ =
      socket(address_->ai_family, address_->ai_socktype, address_->ai_protocol);
}

PosixSocket::~PosixSocket() {
  freeaddrinfo(address_);

  close(socketDescriptor_);
}

auto PosixSocket::BindAndListen() -> bool {
  if (bind(socketDescriptor_, address_->ai_addr, address_->ai_addrlen) == -1) {
    return false;
  }

  static constexpr int backlog = 5;
  return (listen(socketDescriptor_, backlog) == 0);
}

auto PosixSocket::Connect() -> bool {
  return (connect(socketDescriptor_, address_->ai_addr, address_->ai_addrlen) ==
          0);
}

auto PosixSocket::SendMessage(
    const std::unique_ptr<std::vector<PosixSocket::Byte>>& message) -> bool {
  return (send(socketDescriptor_, message->data(), message->size(), 0) != -1);
}

auto PosixSocket::SendMessage(
    const std::unique_ptr<std::vector<PosixSocket::Byte>>& message,
    SocketDescriptor socket_addr) -> bool {
  return (send(socket_addr, message->data(), message->size(), 0) != -1);
}

auto PosixSocket::SendMessageAndCloseClient(
    const std::unique_ptr<std::vector<PosixSocket::Byte>>& message,
    SocketDescriptor socket_addr) -> bool {
  auto res = send(socket_addr, message->data(), message->size(), 0) != -1;
  close(socket_addr);
  return res;
}

auto PosixSocket::Accept() -> SocketDescriptor {
  PosixSocket::SocketDescriptor client_addr = -1;
  if (client_addr = accept(socketDescriptor_, nullptr, nullptr);
      client_addr == -1) {
    throw std::runtime_error("could not accept socket");
  }

  return client_addr;
}

auto PosixSocket::ReceiveMessage(SocketDescriptor client_socket)
    -> std::unique_ptr<std::vector<Byte>> {
  auto buffer =
      std::make_unique<std::vector<Byte>>(std::vector<Byte>(MAX_BUFFER_SIZE));

  const auto& len = recv(client_socket, buffer->data(), buffer->size(), 0);
  buffer->resize(len);

  return buffer;
}

auto PosixSocket::ReceiveMessage() -> std::unique_ptr<std::vector<Byte>> {
  return ReceiveMessage(socketDescriptor_);
}

auto PosixSocket::CreateAddress(const std::string& address, int port,
                                decltype(address_)& addr_out) noexcept -> int {
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

#endif  // POSIX
