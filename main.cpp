#include "Server.h"

auto main() -> int {
  simple_http_server::Server server;

  server.MapUrl("/test", [](const simple_http_server::Server::Request&){
    static constexpr int OK_CODE = 200;
    return simple_http_server::Server::CreateResponse(OK_CODE, "Hello world");
  });

  static constexpr int DEFAULT_PORT = 8080;
  server.Start("0.0.0.0", DEFAULT_PORT);

  return 0;
}
