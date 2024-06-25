#include "DefineSystem.h"

#ifdef WINDOWS

#include "WindowsSocket.h"

#include <memory>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>


namespace simple_http_server {

WindowsSocket::WindowsSocket() {
  wsaLock_.lock();

  if (!wsaStarted_) {
    WSAStartup(MAKEWORD(2,2), &wsaData_);
    wsaStarted_ = true;
  }

  wsaLock_.unlock();

  socketDescriptor_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

WindowsSocket::~WindowsSocket() {
  closesocket(socketDescriptor_);
}

bool WindowsSocket::BindAndListen(const std::string& address, int port) {
  address_ = CreateAddress(address, port);

  if (bind(socketDescriptor_, address_.ai_addr,
           static_cast<int> (address_.ai_addrlen)) == SOCKET_ERROR) {
    return false;
  }

  return (listen(socketDescriptor_, 1) != SOCKET_ERROR);
}

bool WindowsSocket::Connect(std::string address, int port) {
  address_ = CreateAddress(address, port);

  return (connect(socketDescriptor_, address_.ai_addr,
                  static_cast<int> (address_.ai_addrlen)) == 0);
}

bool WindowsSocket::SendMessage(const std::unique_ptr<std::vector<WindowsSocket::Byte>>& message) {
  return (send(socketDescriptor_, reinterpret_cast<const char *>(message->data()), message->size(), 0) != SOCKET_ERROR);
}

bool WindowsSocket::SendMessage(const std::unique_ptr<std::vector<WindowsSocket::Byte>>& message,
                                SocketDescriptor socket_addr) {
  return (send(socket_addr, reinterpret_cast<const char *>(message->data()), message->size(), 0) != -1);
}

bool WindowsSocket::SendMessageAndCloseClient(const std::unique_ptr<std::vector<WindowsSocket::Byte>>& message,
                                              SocketDescriptor socket_addr) {
  auto res = send(socket_addr, reinterpret_cast<const char *>(message->data()), message->size(), 0) != -1;
  shutdown(socket_addr, SD_SEND);
  return res;
}

WindowsSocket::SocketDescriptor WindowsSocket::Accept() {
  auto client_addr = accept(socketDescriptor_, NULL, NULL);
  if (client_addr == INVALID_SOCKET) {
    throw std::runtime_error("could not accept socket");
  }

  return client_addr;
}

auto PosixSocket::ReceiveMessage(SocketDescriptor client_socket) -> std::pair<size_t, std::unique_ptr<std::vector<Byte>>> {
  auto buffer = std::make_unique<std::vector<Byte>>(std::vector<Byte>(MAX_BUFFER_SIZE));

  const auto& len = recv(client_socket, buffer->data(), buffer->size(), 0);

  buffer->resize(len);

  return buffer;
}

auto PosixSocket::ReceiveMessage() -> std::unique_ptr<std::vector<Byte>> {
  return ReceiveMessage(socketDescriptor_);
}

addrinfo WindowsSocket::CreateAddress(const std::string &address, int port) {
  addrinfo hints {};
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  addrinfo * p_res;

  auto ret = getaddrinfo(address.c_str(), std::to_string(port).c_str(), &hints, &p_res);

  (void)ret;

  return *p_res;
}

} // SimpleHttpServer

#endif //WINDOWS
