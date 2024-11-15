#ifndef SIMPLE_HTTP_SERVER_LIBS_SERVER_H_
#define SIMPLE_HTTP_SERVER_LIBS_SERVER_H_

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <regex>
#include <string>
#include <unordered_map>

#include "DefineSystem.h"
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

  class Directory {
   public:
    enum AllowType : std::uint8_t { BLACKLIST, WHITELIST };

   private:
    std::filesystem::path path_;
    HeadersMap headers_;

    AllowType type_;

    std::vector<std::regex> allow_set_;

   public:
    explicit Directory(std::filesystem::path path, HeadersMap headers = {},
                       AllowType type = BLACKLIST,
                       std::vector<std::regex> allow_set = {})
        : path_(std::move(path)),
          headers_(std::move(headers)),
          type_(type),
          allow_set_(std::move(allow_set)) {}

    [[nodiscard]] auto GetPath() const -> decltype(auto) { return path_; }
    [[nodiscard]] auto GetHeaders() const -> decltype(auto) { return headers_; }
    [[nodiscard]] auto GetType() const -> decltype(auto) { return type_; }
    [[nodiscard]] auto GetAllowSet() const -> decltype(auto) {
      return allow_set_;
    }
  };

  void Start();
  void Stop();

  void MapUrl(const std::string& path,
              const std::function<Response(Request)>& function);
  void MapDirectory(const std::string& url, const Directory& directory);

  static auto Render(const std::filesystem::path& file,
                     const std::string& content_type = "",
                     HeadersMap headers = {}) -> Response;

  static auto DeduceContentType(const std::filesystem::path& path)
      -> std::string;

 private:
  std::unique_ptr<ISocket> socket_;
  std::unordered_map<std::string, std::function<Response(Request)>> mappedUrls_;
  std::unordered_map<std::string, Directory> mappedDirectories_;
  volatile std::atomic<bool> running_;

  void HandleClient(const ISocket::SocketDescriptor& client_sock);
};

}  // namespace simple_http_server

#endif  // SIMPLE_HTTP_SERVER_LIBS_SERVER_H_
