#include "Server.h"
#include "util.h"

#include <gtest/gtest.h>
#include <thread>

using namespace simple_http_server;

TEST(Server, CreateAndDumpResponseWithHeaders) {
  std::ostringstream canon_response;

  canon_response << "HTTP/1.1 200 OK" << "\r\n"
                 << "Content-Length: 11" << "\r\n"
                 << "\r\n"
                 << "Hello world";

  auto response = Server::DumpResponse(Server::CreateResponse(200, "Hello world", "", {{"Content-Length", "11"}}));

  EXPECT_EQ(canon_response.str(), response);
}

TEST(Server, CreateAndDumpResponseWithoutHeaders) {
  std::ostringstream canon_response;

  canon_response << "HTTP/1.1 200 OK" << "\r\n"
                 << "Content-Length: 11" << "\r\n"
                 << "\r\n"
                 << "Hello world";

  auto response = Server::DumpResponse(Server::CreateResponse(200, "Hello world"));

  EXPECT_EQ(canon_response.str(), response);
}

TEST(Server, ParseRequest) {
  std::string request = "POST /home.html HTTP/1.1\r\n"
                        "Host: developer.mozilla.org\r\n"
                        "Content-Type: application/x-www-form-urlencoded\r\n"
                        "Content-Length: 27\r\n"
                        "\r\n"
                        "field1=value1&field2=value2";

  auto parsed = Server::ParseRequest(request);

  Server::Request canon = {.type=Server::Request::POST,
                           .url="/home.html",
                           .arguments=Server::ArgumentsMap(),
                           .httpVersion="HTTP/1.1",
                           .headers={{"Host", "developer.mozilla.org"},
                                     {"Content-Type", "application/x-www-form-urlencoded"},
                                     {"Content-Length", "27"}},
                           .body="field1=value1&field2=value2"};

  EXPECT_EQ(canon.type, parsed.type);
  EXPECT_EQ(canon.url, parsed.url);
  EXPECT_EQ(canon.arguments, parsed.arguments);
  EXPECT_EQ(canon.httpVersion, parsed.httpVersion);
  EXPECT_EQ(canon.headers, parsed.headers);
  EXPECT_EQ(canon.body, parsed.body);
}

TEST(Server, ParseRequestWithArguments) {
  std::string request = "GET /home.html?a=b&c=d HTTP/1.1\r\n"
                        "Host: developer.mozilla.org\r\n"
                        "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0\r\n"
                        "\r\n";

  auto parsed = Server::ParseRequest(request);

  Server::Request canon = {.type=Server::Request::GET,
                           .url="/home.html",
                           .arguments={{"a", "b"}, {"c", "d"}},
                           .httpVersion="HTTP/1.1",
                           .headers={{"Host", "developer.mozilla.org"},
                                     {"User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0"}},
                           .body=""};

  EXPECT_EQ(canon.type, parsed.type);
  EXPECT_EQ(canon.url, parsed.url);
  EXPECT_EQ(canon.arguments, parsed.arguments);
  EXPECT_EQ(canon.httpVersion, parsed.httpVersion);
  EXPECT_EQ(canon.headers, parsed.headers);
  EXPECT_EQ(canon.body, parsed.body);
}
