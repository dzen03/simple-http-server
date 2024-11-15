#ifndef SIMPLE_HTTP_SERVER_LIBS_RESPONSE_H_
#define SIMPLE_HTTP_SERVER_LIBS_RESPONSE_H_

#include <cstdint>
#include <string>

#include "Util.h"

namespace simple_http_server {

class Response {
 public:
  Response() = default;

  explicit Response(int statusCode, std::string body = "",
                    HeadersMap headers = HeadersMap(),
                    const std::string& statusMessage = "");
  auto Dump() -> std::string;

  [[nodiscard]] auto Empty() const -> bool { return (statusCode_ == 0); }

  enum HttpStatusCodes : std::uint16_t {
    OK = 200,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    INTERNAL_ERROR = 500
  };

 private:
  std::string httpVersion_ = "HTTP/1.1";
  int statusCode_ = 0;
  std::string statusMessage_;
  HeadersMap headers_;
  std::string body_;

  inline static const std::unordered_map<int, std::string> defaultMessages_{
      {OK, "OK"},
      {NOT_FOUND, "Not Found"},
      {FORBIDDEN, "Forbidden"},
      {INTERNAL_ERROR, "Internal Server Error"}};
};

}  // namespace simple_http_server

#endif  // SIMPLE_HTTP_SERVER_LIBS_RESPONSE_H_
