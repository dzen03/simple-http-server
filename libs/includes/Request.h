#ifndef SIMPLE_HTTP_SERVER_LIBS_REQUEST_H_
#define SIMPLE_HTTP_SERVER_LIBS_REQUEST_H_

#include <cstdint>
#include <string>

#include "Util.h"

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
  explicit Request(const std::string& stringRequest);

 private:
  static auto ParseArguments(const std::string& url_with_args) -> ArgumentsMap;
  static auto DecodeURL(std::string& url_with_args);

  Type type_ = UNKNOWN;
  std::string url_;
  ArgumentsMap arguments_;
  std::string httpVersion_;
  HeadersMap headers_;
  std::string body_;

 public:
  [[nodiscard]] auto GetType() const -> decltype(type_) { return type_; }
  [[nodiscard]] auto GetUrl() const -> decltype(url_) { return url_; }
  [[nodiscard]] auto GetArguments() const -> decltype(arguments_) {
    return arguments_;
  }
  [[nodiscard]] auto GetHttpVersion() const -> decltype(httpVersion_) {
    return httpVersion_;
  }
  [[nodiscard]] auto GetHeaders() const -> decltype(headers_) {
    return headers_;
  }
  [[nodiscard]] auto GetBody() const -> decltype(body_) { return body_; }
};

}  // namespace simple_http_server

#endif  // SIMPLE_HTTP_SERVER_LIBS_REQUEST_H_
