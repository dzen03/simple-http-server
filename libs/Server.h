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
    enum Type {
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
  void Start(const std::string &ip = "0.0.0.0", int port = 8080);
  void Stop();

  void MapUrl(const std::string &path, const std::function<Server::Response(Server::Request)> &function);

  static ArgumentsMap ParseArguments(const std::string &url_with_args);
  static Request ParseRequest(const std::string &stringRequest);
  static Response CreateResponse(int statusCode, const std::string &body = "", const std::string &statusMessage = "",
                                 const HeadersMap &headers = HeadersMap());
  static std::string DumpResponse(const Response &response);
 private:
  std::unique_ptr<ISocket> socket_;
  inline static const std::unordered_map<int, std::string>
      defaultMessages_{{200, "OK"}, {404, "Not Found"}, {500, "Internal Server Error"}};
  std::unordered_map<std::string, std::function<Response(Request)>> mappedUrls_;
  volatile std::atomic<bool> running_;
};

} // simple_http_server

#endif //SIMPLE_HTTP_SERVER_LIBS_SERVER_H_
