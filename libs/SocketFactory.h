#ifndef SIMPLE_HTTP_SERVER_SOCKETFACTORY_H
#define SIMPLE_HTTP_SERVER_SOCKETFACTORY_H

#include "ISocket.h"

namespace simple_http_server {

class SocketFactory {
 public:
  static std::unique_ptr<ISocket> CreateSocket();
};

} // simple_http_server

#endif //SIMPLE_HTTP_SERVER_SOCKETFACTORY_H
