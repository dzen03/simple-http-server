#ifndef SIMPLE_HTTP_SERVER_LIBS_RESPONSE_H_
#define SIMPLE_HTTP_SERVER_LIBS_RESPONSE_H_

#include "Util.h"

#include <string>

namespace simple_http_server {

class Response {
 public:
  Response() = default;

  explicit Response(int statusCode, const std::string &body = "", const HeadersMap &headers = HeadersMap(),
                    const std::string &statusMessage = "");
  auto Dump() -> std::string;

  [[nodiscard]] auto Empty() const -> bool {
    return (statusCode_ == 0);
  }

 private:
  std::string httpVersion_ = "HTTP/1.1";
  int statusCode_ = 0;
  std::string statusMessage_;
  HeadersMap headers_;
  std::string body_;

  inline static const std::unordered_map<int, std::string>
      defaultMessages_{{200, "OK"}, {404, "Not Found"}, {500, "Internal Server Error"}};
};

} // namespace simple_http_server

#endif //SIMPLE_HTTP_SERVER_LIBS_RESPONSE_H_
