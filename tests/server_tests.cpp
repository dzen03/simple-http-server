#include <gtest/gtest.h>

#include <random>
#include <thread>

#include "ISocket.h"
#include "Server.h"
#include "SocketFactory.h"

namespace simple_http_server {

TEST(Server, MapURL) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(
      10000, 65535);  // NOLINT

  const int PORT =
      static_cast<int>(dist(rng));  // TODO(dzen) use kernel to get random port
  // "STATIC" port isn't working properly

  Server server("127.0.0.1", PORT);

  std::thread server_thread([&server]() {
    server.MapUrl("/", [](const simple_http_server::Request&) {
      static constexpr int OK_CODE = 200;
      return simple_http_server::Response(OK_CODE, "test1234片仮名");
    });

    server.Start();
  });

  std::this_thread::sleep_for(std::chrono::seconds(2));
  server.Stop();

  auto client = SocketFactory::CreateSocket("127.0.0.1", PORT);

  const auto& connected = client->Connect();

  if (!connected) {
    server_thread.join();
    ASSERT_TRUE(false);
  }

  const std::string& request = "GET / HTTP/1.1\r\n\r\n";

  EXPECT_TRUE(client->SendMessage(
      std::make_unique<ISocket::Message>(request.begin(), request.end())));

  const auto& resp = client->ReceiveMessage();

  EXPECT_FALSE(resp->empty());

  EXPECT_EQ(std::string(resp->begin(), resp->end()),
            "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: "
            "17\r\n\r\ntest1234片仮名");

  server_thread.join();
}

}  // namespace simple_http_server
