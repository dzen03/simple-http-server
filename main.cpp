#include "Server.h"

using namespace simple_http_server;

int main() {
  Server server;

  server.MapUrl("/test", [](const Server::Request&){
    return Server::CreateResponse(200, "Hello world");
  });

  server.Start();

  return 0;
}
