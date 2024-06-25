#include "Response.h"
#include "Request.h"
#include "Util.h"

#include <gtest/gtest.h>

namespace simple_http_server {

TEST(Response, CreateAndDumpResponseWithHeaders) {
  std::ostringstream canon_response;

  canon_response << "HTTP/1.1 200 OK" << "\r\n"
                 << "Content-Length: 11" << "\r\n"
                 << "\r\n"
                 << "Hello world";

  //NOLINTNEXTLINE(readability-magic-numbers)
  auto response = Response(200, "Hello world", {{"Content-Length", "11"}}).Dump();

  EXPECT_EQ(canon_response.str(), response);
}

TEST(Response, CreateAndDumpResponseWithoutHeaders) {
  std::ostringstream canon_response;

  canon_response << "HTTP/1.1 200 OK" << "\r\n"
                 << "Content-Length: 11" << "\r\n"
                 << "\r\n"
                 << "Hello world";

  //NOLINTNEXTLINE(readability-magic-numbers)
  auto response = Response(200, "Hello world").Dump();

  EXPECT_EQ(canon_response.str(), response);
}
} // namespace simple_http_server
