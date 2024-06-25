#include "Response.h"
#include "Request.h"
#include "Util.h"

#include <gtest/gtest.h>

namespace simple_http_server {
TEST(Request, ParseRequest) {
  const auto& request = "POST /home.html HTTP/1.1\r\n"
                              "Host: developer.mozilla.org\r\n"
                              "Content-Type: application/x-www-form-urlencoded\r\n"
                              "Content-Length: 27\r\n"
                              "\r\n"
                              "field1=value1&field2=value2";

  auto parsed = Request(request);

  EXPECT_EQ(parsed.GetType(), Request::POST);
  EXPECT_EQ(parsed.GetUrl(), "/home.html");
  EXPECT_EQ(parsed.GetArguments(), ArgumentsMap());
  EXPECT_EQ(parsed.GetHttpVersion(), "HTTP/1.1");
  EXPECT_EQ(parsed.GetHeaders(), HeadersMap({{"Host", "developer.mozilla.org"},
                                             {"Content-Type", "application/x-www-form-urlencoded"},
                                             {"Content-Length", "27"}}));
  EXPECT_EQ(parsed.GetBody(), "field1=value1&field2=value2");
}

TEST(Request, ParseRequestWithArguments) {
  const auto& request = "GET /home.html?a=b&c=d HTTP/1.1\r\n"
                              "Host: developer.mozilla.org\r\n"
                              "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0\r\n"
                              "\r\n";

  auto parsed = Request(request);

  EXPECT_EQ(parsed.GetType(), Request::GET);
  EXPECT_EQ(parsed.GetUrl(), "/home.html");
  EXPECT_EQ(parsed.GetArguments(), ArgumentsMap({{"a", "b"}, {"c", "d"}}));
  EXPECT_EQ(parsed.GetHttpVersion(), "HTTP/1.1");
  EXPECT_EQ(parsed.GetHeaders(), HeadersMap({{"Host", "developer.mozilla.org"},
                                             {"User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0"}}));
  EXPECT_EQ(parsed.GetBody(), "");
}
} // namespace simple_http_server
