#ifndef SIMPLE_HTTP_SERVER_LIBS_DIRECTORY_H_
#define SIMPLE_HTTP_SERVER_LIBS_DIRECTORY_H_

#include <cstdint>
#include <filesystem>
#include <regex>

#include "Util.h"

namespace simple_http_server {

class Directory {
 public:
  enum AllowType : std::uint8_t { BLACKLIST, WHITELIST };

 private:
  std::filesystem::path path_;
  HeadersMap headers_;

  AllowType type_;

  std::vector<std::regex> allow_set_;
  inline static const std::vector<std::regex> forced_blacklist_ = {
      std::regex("^.*\\.\\..*$")};  // to block path traversal

 public:
  explicit Directory(std::filesystem::path path, HeadersMap headers = {},
                     AllowType type = BLACKLIST,
                     std::vector<std::regex> allow_set = {})
      : path_(std::move(path)),
        headers_(std::move(headers)),
        type_(type),
        allow_set_(std::move(allow_set)) {}

  [[nodiscard]] auto GetPath() const { return path_; }
  [[nodiscard]] auto GetHeaders() const { return headers_; }
  [[nodiscard]] auto GetType() const { return type_; }
  [[nodiscard]] auto GetAllowSet() const { return allow_set_; }
  [[nodiscard]] auto static GetForcedBlacklist() { return forced_blacklist_; }
};

}  // namespace simple_http_server

#endif  // SIMPLE_HTTP_SERVER_LIBS_DIRECTORY_H_
