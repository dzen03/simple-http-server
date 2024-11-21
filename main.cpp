#include <regex>

#include "Directory.h"
#include "Logger.h"
#include "Request.h"
#include "Response.h"
#include "Server.h"

auto main() -> int {
  static constexpr int DEFAULT_PORT = 8080;
  simple_http_server::Server server("0.0.0.0", DEFAULT_PORT,
                                    simple_http_server::DEBUG);

  server.MapUrl(
      "/plain",
      [](const simple_http_server::Request&) {
        return simple_http_server::Response(
            simple_http_server::Response::HttpStatusCodes::OK,
            "Hello world in plain text");
      },
      true);

  server.MapUrl("/", [](const simple_http_server::Request&) {
    return simple_http_server::Server::Render("index.html");
  });

  server.MapDirectory("/dir",
                      simple_http_server::Directory(
                          "./", {}, simple_http_server::Directory::WHITELIST,
                          {std::regex("^.*.log$")}));

  server.Start();

  return 0;
}
