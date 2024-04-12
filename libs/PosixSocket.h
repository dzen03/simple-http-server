#ifndef SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_
#define SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_

#include "ISocket.h"

#include <arpa/inet.h>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <vector>

namespace simple_http_server {

class PosixSocket : protected ISocket {
 public:
  PosixSocket();
  ~PosixSocket() override;

  bool BindAndListen(std::string address, int port) override;
  bool Connect(std::string address, int port) override;
  bool SendMessage(const std::shared_ptr<std::vector<Byte>>& message) override;
  bool SendMessage(const std::shared_ptr<std::vector<Byte>>& message,
                   SocketDescriptor socket_addr) override;
  bool SendMessageAndCloseClient(const std::shared_ptr<std::vector<Byte>>& message,
                   SocketDescriptor socket_addr) override;
  std::optional<SocketDescriptor> Accept() override;
  int ReceiveMessage(SocketDescriptor clint_socket, std::shared_ptr<std::vector<Byte>> message) override;
 private:
  SocketDescriptor socketDescriptor_;
  sockaddr_in address_;

  static sockaddr_in CreateAddress(const std::string& address, int port);
};

} // SimpleHttpServer

#endif //SIMPLE_HTTP_SERVER_LIBS_POSIXSOCKET_H_
