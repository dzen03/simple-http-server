#ifndef SIMPLE_HTTP_SERVER_SOCKETFACTORY_H
#define SIMPLE_HTTP_SERVER_SOCKETFACTORY_H

#include "ISocket.h"

namespace simple_http_server {

class SocketFactory {
 public:
  static auto CreateSocket(const std::string& address,
                           int port) -> std::unique_ptr<ISocket>;
};

}  // namespace simple_http_server

#endif  // SIMPLE_HTTP_SERVER_SOCKETFACTORY_H
