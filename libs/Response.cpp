#include "Response.h"

#include <memory>
#include <sstream>

#include "Util.h"

namespace simple_http_server {

Response::Response(int statusCode, const std::string& body,
                   const HeadersMap& headers,
                   const std::string& statusMessage) {
  statusCode_ = statusCode;
  statusMessage_ = (statusMessage.empty() ? defaultMessages_.at(statusCode_)
                                          : statusMessage);
  headers_ = headers;
  body_ = body;

  if (!headers_.contains("Content-Length")) {
    headers_.emplace("Content-Length", std::to_string(body_.length()));
  }

  if (!headers_.contains(
          "Connection")) {  // TODO(dzen) create keep-alive mechanism
    headers_.emplace("Connection", "close");
  }
}

auto Response::Dump() -> std::string {
  std::ostringstream string_response;

  string_response << httpVersion_ << " " << statusCode_ << " " << statusMessage_
                  << "\r\n";

  for (const auto& header : headers_) {
    string_response << header.first << ": " << header.second << "\r\n";
  }

  if (headers_.empty()) {
    string_response << "\r\n";
  }

  string_response << "\r\n" << body_;

  return string_response.str();
}

}  // namespace simple_http_server
