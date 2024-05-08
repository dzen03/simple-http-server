#include "SocketFactory.h"
#include "ISocket.h"
#include <thread>
#include <gtest/gtest.h>

using namespace simple_http_server;

TEST(Socket, BasicPing) {
  std::string test_message_string = "test1234";
  std::vector<ISocket::Byte> test_message(test_message_string.begin(), test_message_string.end());

  auto buffer = std::make_shared<ISocket::Message>(ISocket::Message(test_message.size()));

  auto server = SocketFactory::CreateSocket();
  auto client = SocketFactory::CreateSocket();

  EXPECT_TRUE(server->BindAndListen("127.0.0.1", 12345));

  std::thread server_thread([&server, &buffer]() {
    auto client_sock = server->Accept();

    EXPECT_TRUE(client_sock);
    EXPECT_TRUE(server->ReceiveMessage(client_sock.value(), buffer) > 0);
  });

  // TODO think about race here

  EXPECT_TRUE(client->Connect("127.0.0.1", 12345));
  EXPECT_TRUE(client->SendMessage(std::make_shared<ISocket::Message>(test_message)));

  server_thread.join();

  EXPECT_EQ(*buffer, test_message);
}
