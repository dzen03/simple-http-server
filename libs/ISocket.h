#ifndef SIMPLE_HTTP_SERVER_LIBS_ISOCKET_H_
#define SIMPLE_HTTP_SERVER_LIBS_ISOCKET_H_

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "util.h"

namespace simple_http_server {

class ISocket {
 public:
  using Byte = uint8_t;
  using Message = std::vector<Byte>;
#if defined(WINDOWS)
  using SocketDescriptor = SOCKET;
#elif defined(POSIX)
  using SocketDescriptor = int;
#endif

  ISocket() = default;
  virtual ~ISocket() = default;

  virtual bool Connect(std::string address, int port) = 0;
  virtual bool SendMessage(const std::shared_ptr<std::vector<Byte>>& message,
                           SocketDescriptor socket_addr) = 0;
  virtual bool SendMessageAndCloseClient(const std::shared_ptr<std::vector<Byte>>& message,
                           SocketDescriptor socket_addr) = 0;
  virtual bool SendMessage(const std::shared_ptr<std::vector<Byte>>& message) = 0;

  virtual bool BindAndListen(std::string address, int port) = 0;

  virtual std::optional<SocketDescriptor> Accept() = 0;
  virtual int ReceiveMessage(SocketDescriptor clint_socket,
                         std::shared_ptr<std::vector<Byte>> message) = 0;
};

} // SimpleHttpServer

#endif //SIMPLE_HTTP_SERVER_LIBS_ISOCKET_H_
