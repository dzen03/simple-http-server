#ifndef SIMPLE_HTTP_SERVER_LIBS_SERVER_H_
#define SIMPLE_HTTP_SERVER_LIBS_SERVER_H_

#include "DefineSystem.h"
#include "ISocket.h"
#include "Request.h"
#include "Response.h"

#include <atomic>
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>

namespace simple_http_server {

class Server {
 public:
  Server();

  Server(const Server &server) = delete;
  auto operator=(Server& other) = delete;
  Server(Server &&server) = delete;
  auto operator=(Server&& other) = delete;

  void Start(const std::string &ip_addr, int port);
  void Stop();

  void MapUrl(const std::string &path, const std::function<Response(Request)> &function);
  void MapDirectory(const std::string &url, const std::filesystem::path &directory);

  static auto Render(const std::filesystem::path &file, const std::string &content_type = "") -> Response;

  static auto DeduceContentType(const std::filesystem::path &path) -> std::string;
 private:
  std::unique_ptr<ISocket> socket_;
  std::unordered_map<std::string, std::function<Response(Request)>> mappedUrls_;
  std::unordered_map<std::string, std::filesystem::path> mappedDirectories_;
  volatile std::atomic<bool> running_;

  void HandleClient(const ISocket::SocketDescriptor& client_sock);
};

} // namespace simple_http_server

#endif //SIMPLE_HTTP_SERVER_LIBS_SERVER_H_
