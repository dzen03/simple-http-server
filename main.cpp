#include "Server.h"

auto main() -> int {
  simple_http_server::Server server;

  server.MapUrl("/plain", [](const simple_http_server::Request&) {
    static constexpr int OK_CODE = 200;
    return simple_http_server::Response(OK_CODE, "Hello world in plain text");
  });

  server.MapUrl("/", [](const simple_http_server::Request&) {
    return simple_http_server::Server::Render("index.html");
  });

  server.MapDirectory("/dir", "./");

  static constexpr int DEFAULT_PORT = 8080;
  server.Start("0.0.0.0", DEFAULT_PORT);

  return 0;
}
