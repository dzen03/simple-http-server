#include <gtest/gtest.h>

#include "Response.h"
#include "Util.h"

namespace simple_http_server {
namespace {
constexpr int OK_CODE = 200;
}  // namespace

TEST(Response, CreateAndDumpResponseWithHeaders) {
  std::ostringstream canon_response;

  // FIXME(dzen) remove dependency of unordered map order :|
  canon_response << "HTTP/1.1 200 OK" << "\r\n"
                 << "Connection: close" << "\r\n"
                 << "Content-Length: 11" << "\r\n"
                 << "\r\n"
                 << "Hello world";

  auto response =
      Response(OK_CODE, "Hello world", {{"Content-Length", "11"}}).Dump();

  EXPECT_EQ(canon_response.str(), response);
}

TEST(Response, CreateAndDumpResponseWithoutHeaders) {
  std::ostringstream canon_response;

  // FIXME(dzen) remove dependency of unordered map order :|
  canon_response << "HTTP/1.1 200 OK" << "\r\n"
                 << "Connection: close" << "\r\n"
                 << "Content-Length: 11" << "\r\n"
                 << "\r\n"
                 << "Hello world";

  // NOLINTNEXTLINE(readability-magic-numbers)
  auto response = Response(OK_CODE, "Hello world").Dump();

  EXPECT_EQ(canon_response.str(), response);
}
}  // namespace simple_http_server
