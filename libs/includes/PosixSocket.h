#ifndef SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_
#define SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_

#include "DefineSystem.h"

#ifdef POSIX

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <memory>
#include <string>
#include <vector>

#include "ISocket.h"

namespace simple_http_server {

class PosixSocket : public ISocket {
 public:
  PosixSocket(const std::string& address, int port);
  ~PosixSocket() override;

  PosixSocket(const PosixSocket& source) = default;
  PosixSocket(PosixSocket&& source) = default;

  auto operator=(const PosixSocket& source) -> PosixSocket& = default;
  auto operator=(PosixSocket&& source) -> PosixSocket& = default;

  auto BindAndListen() -> bool override;
  auto Connect() -> bool override;
  auto SendMessage(const std::unique_ptr<std::vector<Byte>>& message)
      -> bool override;
  auto SendMessage(const std::unique_ptr<std::vector<Byte>>& message,
                   SocketDescriptor socket_addr) -> bool override;
  auto SendMessageAndCloseClient(
      const std::unique_ptr<std::vector<Byte>>& message,
      SocketDescriptor socket_addr) -> bool override;
  auto Accept() -> SocketDescriptor override;
  auto ReceiveMessage(SocketDescriptor client_socket)
      -> std::unique_ptr<std::vector<Byte>> override;
  auto ReceiveMessage() -> std::unique_ptr<std::vector<Byte>> override;

 private:
  SocketDescriptor socketDescriptor_;
  addrinfo* address_ = nullptr;

  static auto CreateAddress(const std::string& address, int port,
                            decltype(address_)& addr_out) noexcept -> int;
};

}  // namespace simple_http_server

#endif  // POSIX

#endif  // SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_
