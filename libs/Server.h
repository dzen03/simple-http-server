#ifndef SIMPLE_HTTP_SERVER_LIBS_SERVER_H_
#define SIMPLE_HTTP_SERVER_LIBS_SERVER_H_

#include "PosixSocket.h"
#include "util.h"

#include <string>
#include <unordered_map>
#include <functional>

namespace simple_http_server {

class Server {
 public:
  using HeadersMap = std::unordered_map<std::string, std::string>;
  struct Request {
    enum Type {
      GET,
      POST,
      // add here new types such as DELETE and etc
      UNKNOWN
    } type;
    std::string url;
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
  Server() = default;
  void Start();
  static Request ParseRequest(const std::string& stringRequest);
  Response CreateResponse(int statusCode, const std::string& body="", const std::string& statusMessage="",
                          const HeadersMap& headers=HeadersMap());
  static std::string DumpResponse(const Response& response);

 private:
#if defined(POSIX)
  PosixSocket socket_;
#elif defined(WINDOWS)
#error Windows not implemented yet
#endif
  const std::unordered_map<int, std::string> defaultMessages_ = {{200, "OK"}, {404, "Not Found"}, {500, "Internal Server Error"}};
  std::unordered_map<std::string, std::function<Response(Request)>> mappedUrls_;
};

} // simple_http_server

#endif //SIMPLE_HTTP_SERVER_LIBS_SERVER_H_
