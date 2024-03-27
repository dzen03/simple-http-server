#include "PosixSocket.h"

#include <arpa/inet.h>
#include <memory>
#include <netinet/in.h>
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

bool PosixSocket::BindAndListen(std::string address, int port) {
  address_ = CreateAddress(address, port);

  if (bind(socketDescriptor_, reinterpret_cast<sockaddr*> (&address_),
           sizeof(address_)) == -1) {
    return false;
  }

  return (listen(socketDescriptor_, 1) == 0);
}

bool PosixSocket::Connect(std::string address, int port) {
  address_ = CreateAddress(address, port);

  return (connect(socketDescriptor_, reinterpret_cast<sockaddr*> (&address_),
                  sizeof(address_)) == 0);
}

bool PosixSocket::SendMessage(const std::shared_ptr<std::vector<PosixSocket::Byte>>& message) {
  return (send(socketDescriptor_, message->data(), message->size(), 0) != -1);
}

bool PosixSocket::SendMessage(const std::shared_ptr<std::vector<PosixSocket::Byte>>& message,
                              SocketDescriptor socket_addr) {
  return (send(socket_addr, message->data(), message->size(), 0) != -1);
}

std::optional<PosixSocket::SocketDescriptor> PosixSocket::Accept() {
  socklen_t address_size = sizeof(address_);

  PosixSocket::SocketDescriptor client_addr;
  if (client_addr = accept(socketDescriptor_, reinterpret_cast<sockaddr *>(&address_),
                           &address_size); client_addr == -1) {
    return std::nullopt;
  }

  return client_addr;
}

bool PosixSocket::ReceiveMessage(PosixSocket::SocketDescriptor clint_socket,
                                 std::shared_ptr<std::vector<PosixSocket::Byte>> message) {
  if (recv(clint_socket, message->data(), message->size(), 0) < 0) {
    return false;
  }

  return true;
}


sockaddr_in PosixSocket::CreateAddress(const std::string &address, int port) {
  sockaddr_in res{};
  res.sin_family = AF_INET;
  res.sin_port = htons(port);
  res.sin_addr.s_addr = inet_addr(address.c_str());

  return res;
}

} // SimpleHttpServer