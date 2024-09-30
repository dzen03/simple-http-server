#include <gtest/gtest.h>

#include "Request.h"
#include "Response.h"
#include "Util.h"

namespace simple_http_server {
TEST(Request, Parse) {
  const auto& request =
      "POST /home.html HTTP/1.1\r\n"
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
  EXPECT_EQ(parsed.GetHeaders(),
            HeadersMap({{"Host", "developer.mozilla.org"},
                        {"Content-Type", "application/x-www-form-urlencoded"},
                        {"Content-Length", "27"}}));
  EXPECT_EQ(parsed.GetBody(), "field1=value1&field2=value2");
}

TEST(Request, ParseWithArguments) {
  const auto& request =
      "GET /home.html?a=b&c=d HTTP/1.1\r\n"
      "Host: developer.mozilla.org\r\n"
      "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) "
      "Gecko/20100101 Firefox/50.0\r\n"
      "\r\n";

  auto parsed = Request(request);

  EXPECT_EQ(parsed.GetType(), Request::GET);
  EXPECT_EQ(parsed.GetUrl(), "/home.html");
  EXPECT_EQ(parsed.GetArguments(), ArgumentsMap({{"a", "b"}, {"c", "d"}}));
  EXPECT_EQ(parsed.GetHttpVersion(), "HTTP/1.1");
  EXPECT_EQ(parsed.GetHeaders(),
            HeadersMap({{"Host", "developer.mozilla.org"},
                        {"User-Agent",
                         "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; "
                         "rv:50.0) Gecko/20100101 Firefox/50.0"}}));
  EXPECT_EQ(parsed.GetBody(), "");
}

TEST(Request, ParseInvalidHeaders) {
  const auto& request =
      "CONNECT google.com HTTP/1.1\r\n"
      "Host:\r\n"
      "User-Agent: \r\n"
      "\r\n";

  auto parsed = Request(request);

  EXPECT_EQ(parsed.GetType(), Request::UNKNOWN);
  EXPECT_EQ(parsed.GetUrl(), "google.com");
  EXPECT_EQ(parsed.GetArguments(), ArgumentsMap({}));
  EXPECT_EQ(parsed.GetHttpVersion(), "HTTP/1.1");
  EXPECT_EQ(parsed.GetHeaders(), HeadersMap({}));
  EXPECT_EQ(parsed.GetBody(), "");
}

TEST(Request, ParseInvalidHeadersWithValid) {
  const auto& request =
      "GET /manager/html HTTP/1.1\r\n"
      "Host:\r\n"
      "Connection: close\r\n"
      "User_Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 "
      "(KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36\r\n"
      "\r\n";

  auto parsed = Request(request);

  EXPECT_EQ(parsed.GetType(), Request::GET);
  EXPECT_EQ(parsed.GetUrl(), "/manager/html");
  EXPECT_EQ(parsed.GetArguments(), ArgumentsMap({}));
  EXPECT_EQ(parsed.GetHttpVersion(), "HTTP/1.1");
  EXPECT_EQ(
      parsed.GetHeaders(),
      HeadersMap({{"Connection", "close"},
                  {"User_Agent",
                   "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 "
                   "(KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36"}}));
  EXPECT_EQ(parsed.GetBody(), "");
}

}  // namespace simple_http_server
