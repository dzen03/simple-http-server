#ifndef SIMPLE_HTTP_SERVER_LIBS_SERVER_H_
#define SIMPLE_HTTP_SERVER_LIBS_SERVER_H_

#include "ISocket.h"
#include "util.h"

#include <atomic>
#include <functional>
#include <string>
#include <unordered_map>

namespace simple_http_server {

class Server {
 public:
  using HeadersMap = std::unordered_map<std::string, std::string>;
  using ArgumentsMap = std::unordered_map<std::string, std::string>;

  struct Request {
    enum Type : std::uint8_t {
      GET,
      POST,
      // add here new types such as DELETE and etc
      UNKNOWN
    } type;
    std::string url;
    ArgumentsMap arguments;
    std::string httpVersion;
    HeadersMap headers;
    std::string body;
  };
  struct Response {
    std::string httpVersion;
    int statusCode;
    std::string statusMessage;
    HeadersMap headers;
    std::string body;
  };
  Server();
  void Start(const std::string &ip_addr, int port);
  void Stop();

  void MapUrl(const std::string &path, const std::function<Server::Response(Server::Request)> &function);

  static auto ParseArguments(const std::string &url_with_args) -> ArgumentsMap;
  static auto ParseRequest(const std::string &stringRequest) -> Request;
  static auto CreateResponse(int statusCode, const std::string &body = "", const std::string &statusMessage = "",
                                 const HeadersMap &headers = HeadersMap()) -> Response;
  static auto DumpResponse(const Response &response) -> std::string;
 private:
  std::unique_ptr<ISocket> socket_;
  inline static const std::unordered_map<int, std::string>
      defaultMessages_{{200, "OK"}, {404, "Not Found"}, {500, "Internal Server Error"}};
  std::unordered_map<std::string, std::function<Response(Request)>> mappedUrls_;
  volatile std::atomic<bool> running_;
};

} // namespace simple_http_server

#endif //SIMPLE_HTTP_SERVER_LIBS_SERVER_H_
