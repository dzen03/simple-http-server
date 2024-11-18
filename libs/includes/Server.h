#ifndef SIMPLE_HTTP_SERVER_LIBS_SERVER_H_
#define SIMPLE_HTTP_SERVER_LIBS_SERVER_H_

#include <atomic>
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>

#include "DefineSystem.h"
#include "Directory.h"
#include "ISocket.h"
#include "Logger.h"
#include "Request.h"
#include "Response.h"
#include "Util.h"

namespace simple_http_server {

class Server {
 public:
#ifdef NDEBUG
#define DEFAULT_LVL WARNING
#else
#define DEFAULT_LVL DEBUG
#endif
  Server(const std::string& ip_addr, int port, Level logLevel = DEFAULT_LVL);
  ~Server() = default;

  Server(const Server& server) = delete;
  auto operator=(Server& other) = delete;
  Server(Server&& server) = delete;
  auto operator=(Server&& other) = delete;

  void Start();
  void Stop();

  void MapUrl(const std::string& path,
              const std::function<Response(Request)>& function,
              bool include_children = false);
  void MapDirectory(const std::string& url, const Directory& directory);

  static auto Render(const std::filesystem::path& file,
                     const std::string& content_type = "",
                     HeadersMap headers = {}) -> Response;

  static auto DeduceContentType(const std::filesystem::path& path)
      -> std::string;

 private:
  std::unique_ptr<ISocket> socket_;
  std::unordered_map<std::string,
                     std::pair<std::function<Response(Request)>, bool>>
      mappedUrls_;
  std::unordered_map<std::string, Directory> mappedDirectories_;
  volatile std::atomic<bool> running_;

  void HandleClient(const ISocket::SocketDescriptor& client_sock);

  auto TryMappedUrls(const Request& request) -> Response;
  auto TryRenderFile(const Request& request) -> Response;
};

}  // namespace simple_http_server

#endif  // SIMPLE_HTTP_SERVER_LIBS_SERVER_H_
