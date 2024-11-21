#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "ISocket.h"
#include "SocketFactory.h"

namespace simple_http_server {

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(Socket, BasicPong) {
  std::string test_message_string = "test1234片仮名";
  const std::vector<ISocket::Byte> test_message(test_message_string.begin(),
                                                test_message_string.end());

  auto buffer =
      std::make_unique<ISocket::Message>(ISocket::Message(test_message.size()));

  constexpr int port = 12345;
  auto server = SocketFactory::CreateSocket("127.0.0.1", port);
  auto client = SocketFactory::CreateSocket("127.0.0.1", port);

  ASSERT_TRUE(server->BindAndListen());

  std::thread server_thread([&server, &test_message]() {
    auto client_sock = server->Accept();

    const auto& received = server->ReceiveMessage(client_sock);
    EXPECT_FALSE(received->empty());

    EXPECT_EQ(*received, test_message);
  });

  EXPECT_TRUE(client->Connect());
  EXPECT_TRUE(
      client->SendMessage(std::make_unique<ISocket::Message>(test_message)));

  server_thread.join();
}
}  // namespace simple_http_server
