#include <iostream>
#include <vector>

#include "PosixSocket.h"

using namespace simple_http_server;

int main() {
  PosixSocket sock;
  std::cout << sock.BindAndListen("0.0.0.0", 10000) << " " << std::endl;
  perror("Connection error");

  std::vector<ISocket::Byte> buffer(1000);
  auto client_sock = sock.Accept();
  sock.ReceiveMessage(client_sock.value(), std::make_shared<std::vector<ISocket::Byte>>(buffer));

  std::cout << std::string(buffer.begin(), buffer.end());

  return 0;
}
