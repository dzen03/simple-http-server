#ifndef SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_
#define SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_

#include "util.h"

#ifdef POSIX

#include "ISocket.h"

#include <arpa/inet.h>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <vector>

namespace simple_http_server {

class PosixSocket : public ISocket {
 public:
  PosixSocket();
  ~PosixSocket() override;

  PosixSocket(const PosixSocket& source) = default;
  PosixSocket(PosixSocket&& source) = default;

  auto operator=(const PosixSocket& source) -> PosixSocket& = default;
  auto operator=(PosixSocket&& source) -> PosixSocket& = default;

  auto BindAndListen(std::string address, int port) -> bool override;
  auto Connect(std::string address, int port) -> bool override;
  auto SendMessage(const std::shared_ptr<std::vector<Byte>> &message) -> bool override;
  auto SendMessage(const std::shared_ptr<std::vector<Byte>> &message,
                   SocketDescriptor socket_addr) -> bool override;
  auto SendMessageAndCloseClient(const std::shared_ptr<std::vector<Byte>> &message,
                                 SocketDescriptor socket_addr) -> bool override;
  auto Accept() -> std::optional<SocketDescriptor> override;
  auto ReceiveMessage(SocketDescriptor clint_socket, std::shared_ptr<std::vector<Byte>> message) -> size_t override;
 private:
  SocketDescriptor socketDescriptor_;
  sockaddr_in address_{};

  static auto CreateAddress(const std::string &address, int port) -> sockaddr_in;
};

} // namespace simple_http_server

#endif // POSIX

#endif //SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_
