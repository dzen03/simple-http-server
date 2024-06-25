#ifndef SIMPLE_HTTP_SERVER_LIBS_REQUEST_H_
#define SIMPLE_HTTP_SERVER_LIBS_REQUEST_H_

#include "Util.h"

#include <string>

namespace simple_http_server {

class Request {
 public:
  enum Type : std::uint8_t {
    GET,
    POST,
    // add here new types such as DELETE and etc
    UNKNOWN
  };

  Request() = default;
  explicit Request(const std::string &stringRequest);


 private:
  static auto ParseArguments(const std::string &url_with_args) -> ArgumentsMap;

  Type type_;
  std::string url_;
  ArgumentsMap arguments_;
  std::string httpVersion_;
  HeadersMap headers_;
  std::string body_;

 public:
  auto GetType() -> decltype(type_) {
    return type_;
  }
  auto GetUrl() -> decltype(url_) {
    return url_;
  }
  auto GetArguments() -> decltype(arguments_) {
    return arguments_;
  }
  auto GetHttpVersion() -> decltype(httpVersion_) {
    return httpVersion_;
  }
  auto GetHeaders() -> decltype(headers_) {
    return headers_;
  }
  auto GetBody() -> decltype(body_) {
    return body_;
  }

};

} // namespace simple_http_server

#endif //SIMPLE_HTTP_SERVER_LIBS_REQUEST_H_
