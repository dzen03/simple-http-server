#ifndef SIMPLE_HTTP_SERVER_LIBS_ISOCKET_H_
#define SIMPLE_HTTP_SERVER_LIBS_ISOCKET_H_

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "DefineSystem.h"

namespace simple_http_server {

class ISocket {
 public:
  using Byte = uint8_t;
  using Message = std::vector<Byte>;
  using SocketDescriptor = int;

  static constexpr int MAX_BUFFER_SIZE = 8192;

  ISocket() = default;
  virtual ~ISocket() = default;

  ISocket(const ISocket& source) = default;
  ISocket(ISocket&& source) = default;

  auto operator=(const ISocket& source) -> ISocket& = default;
  auto operator=(ISocket&& source) -> ISocket& = default;

  virtual auto Connect(std::string address, int port) -> bool = 0;
  virtual auto SendMessage(const std::unique_ptr<std::vector<Byte>>& message,
                           SocketDescriptor socket_addr) -> bool = 0;
  virtual auto SendMessageAndCloseClient(
      const std::unique_ptr<std::vector<Byte>>& message,
      SocketDescriptor socket_addr) -> bool = 0;
  virtual auto SendMessage(const std::unique_ptr<std::vector<Byte>>& message)
      -> bool = 0;

  virtual auto BindAndListen(const std::string& address, int port) -> bool = 0;

  virtual auto Accept() -> SocketDescriptor = 0;

  virtual auto ReceiveMessage() -> std::unique_ptr<std::vector<Byte>> = 0;
  virtual auto ReceiveMessage(SocketDescriptor client_socket)
      -> std::unique_ptr<std::vector<Byte>> = 0;
};

}  // namespace simple_http_server

#endif  // SIMPLE_HTTP_SERVER_LIBS_ISOCKET_H_
