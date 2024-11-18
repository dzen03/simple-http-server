#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "ISocket.h"
#include "Logger.h"
#include "Request.h"
#include "Server.h"
#include "SocketFactory.h"

namespace simple_http_server {

namespace {
const std::vector<std::string> results = {
    "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: "
    "17\r\n\r\ntest1234片仮名",
    "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Length: 0\r\n\r\n"};
}  // namespace

class ServerTest : public testing::TestWithParam<std::pair<std::string, int>> {
 protected:
  static void SetUpTestSuite() {
    if (server != nullptr) {
      return;
    }
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(
        10000, 65535);  // NOLINT

    port = static_cast<int>(
        dist(rng));  // TODO(dzen) use kernel to get random port
    // "STATIC" port isn't working properly

    std::cout << "Starting server; " << NAMED_OUTPUT(port) << "\n";
    std::cout.flush();

    server = new Server("127.0.0.1", port);  // NOLINT

    server_thread = std::thread([]() {
      server->MapUrl(
          "/true",
          [](const simple_http_server::Request&) {
            return simple_http_server::Response(
                simple_http_server::Response::OK, "test1234片仮名");
          },
          true);
      server->MapUrl(
          "/false",
          [](const simple_http_server::Request&) {
            return simple_http_server::Response(
                simple_http_server::Response::OK, "test1234片仮名");
          },
          false);

      server->Start();
    });
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  static void TearDownTestSuite() {
    server->Stop();
    // FIXME(dzen) fix after rewriting server shutdown
    server_thread.detach();
    // delete server;  // NOLINT
    // server = nullptr;
  }

  static std::thread server_thread;
  static int port;
  static Server* server;
};
std::thread ServerTest::server_thread;
int ServerTest::port = 0;
Server* ServerTest::server = nullptr;

TEST_P(ServerTest, TestMappings) {
  auto client = SocketFactory::CreateSocket("127.0.0.1", port);

  const auto& connected = client->Connect();

  ASSERT_TRUE(connected);

  const std::string& request = "GET " + GetParam().first + " HTTP/1.1\r\n\r\n";

  EXPECT_TRUE(client->SendMessage(
      std::make_unique<ISocket::Message>(request.begin(), request.end())));

  auto resp = client->ReceiveMessage();

  EXPECT_FALSE(resp->empty());

  EXPECT_EQ(std::string(resp->begin(), resp->end()),
            results[GetParam().second]);
}

INSTANTIATE_TEST_SUITE_P(, ServerTest,
                         testing::Values(std::make_pair("/false", 0),
                                         std::make_pair("/true", 0),
                                         std::make_pair("/true/1234", 0),
                                         std::make_pair("/false/1234", 1)));

}  // namespace simple_http_server
