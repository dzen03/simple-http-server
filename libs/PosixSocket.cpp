#include "DefineSystem.h"

#ifdef POSIX

#include "PosixSocket.h"

#include <arpa/inet.h>
#include <memory>
#include <netinet/in.h>
#include <optional>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace simple_http_server {

PosixSocket::PosixSocket() {
  socketDescriptor_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

PosixSocket::~PosixSocket() {
  close(socketDescriptor_);
}

auto PosixSocket::BindAndListen(const std::string& address, int port) -> bool {
  address_ = CreateAddress(address, port);

  if (bind(socketDescriptor_, reinterpret_cast<sockaddr *> (&address_),
           sizeof(address_)) == -1) {
    return false;
  }

  return (listen(socketDescriptor_, 1) == 0);
}

auto PosixSocket::Connect(std::string address, int port) -> bool {
  address_ = CreateAddress(address, port);

  return (connect(socketDescriptor_, reinterpret_cast<sockaddr *> (&address_),
                  sizeof(address_)) == 0);
}

auto PosixSocket::SendMessage(const std::unique_ptr<std::vector<PosixSocket::Byte>> &message) -> bool {
  return (send(socketDescriptor_, message->data(), message->size(), 0) != -1);
}

auto PosixSocket::SendMessage(const std::unique_ptr<std::vector<PosixSocket::Byte>> &message,
                              SocketDescriptor socket_addr) -> bool {
  return (send(socket_addr, message->data(), message->size(), 0) != -1);
}

auto PosixSocket::SendMessageAndCloseClient(const std::unique_ptr<std::vector<PosixSocket::Byte>> &message,
                                            SocketDescriptor socket_addr) -> bool {
  auto res = send(socket_addr, message->data(), message->size(), 0) != -1;
  close(socket_addr);
  return res;
}

auto PosixSocket::Accept() -> SocketDescriptor {
  socklen_t address_size = sizeof(address_);

  PosixSocket::SocketDescriptor client_addr = -1;
  if (client_addr = accept(socketDescriptor_, reinterpret_cast<sockaddr *>(&address_),
                           &address_size); client_addr == -1) {
    throw std::runtime_error("could not accept socket");
  }

  return client_addr;
}

auto PosixSocket::ReceiveMessage(SocketDescriptor client_socket) -> std::unique_ptr<std::vector<Byte>> {
  auto buffer = std::make_unique<std::vector<Byte>>(std::vector<Byte>(MAX_BUFFER_SIZE));

  const auto& len = recv(client_socket, buffer->data(), buffer->size(), 0);
  buffer->resize(len);

  return buffer;
}

auto PosixSocket::ReceiveMessage() -> std::unique_ptr<std::vector<Byte>> {
  return ReceiveMessage(socketDescriptor_);
}

auto PosixSocket::CreateAddress(const std::string &address, int port) noexcept -> sockaddr_in {
  sockaddr_in res{};
  res.sin_family = AF_INET;
  res.sin_port = htons(port);
  res.sin_addr.s_addr = inet_addr(address.c_str());

  return res;
}

} // namespace simple_http_server

#endif //POSIX
