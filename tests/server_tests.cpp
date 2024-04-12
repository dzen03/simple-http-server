#include "util.h"

#include "Server.h"
#include <thread>
#include <gtest/gtest.h>

using namespace simple_http_server;

TEST(Server, CreateAndDumpResponseWithHeaders) {
  std::ostringstream canon_response;

  canon_response << "HTTP/1.1 200 OK" << "\r\n"
           << "Content-Length: 11" << "\r\n"
           << "\r\n"
           << "Hello world";

  Server server;
  auto response = Server::DumpResponse(server.CreateResponse(200, "Hello world", "", {{"Content-Length", "11"}}));

  EXPECT_EQ(canon_response.str(), response);
}

TEST(Server, CreateAndDumpResponseWithoutHeaders) {
  std::ostringstream canon_response;

  canon_response << "HTTP/1.1 200 OK" << "\r\n"
                 << "Content-Length: 11" << "\r\n"
                 << "\r\n"
                 << "Hello world";

  Server server;
  auto response = Server::DumpResponse(server.CreateResponse(200, "Hello world"));

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
                           .httpVersion="HTTP/1.1",
                           .headers={{"Host", "developer.mozilla.org"},
                                     {"Content-Type", "application/x-www-form-urlencoded"},
                                     {"Content-Length", "27"}},
                           .body="field1=value1&field2=value2"};

  EXPECT_EQ(canon.type, parsed.type);
  EXPECT_EQ(canon.url, parsed.url);
  EXPECT_EQ(canon.httpVersion, parsed.httpVersion);
  EXPECT_EQ(canon.headers, parsed.headers);
  EXPECT_EQ(canon.body, parsed.body);
}
