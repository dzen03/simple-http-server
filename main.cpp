#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <unistd.h>

#include "PosixSocket.h"

using namespace simple_http_server;

int main() {
  PosixSocket sock;
  std::cout << sock.BindAndListen("0.0.0.0", 10000) << " " << std::endl;

  auto buffer = std::make_shared<ISocket::Message>(ISocket::Message (1000));
  auto client_sock = sock.Accept();
  int recieved_len = sock.ReceiveMessage(client_sock.value(), buffer);

  std::string request = std::string(buffer->begin(), buffer->end());
  request.resize(recieved_len);
  std::cout << recieved_len << std::endl << request;

  std::stringstream response;

  response << "HTTP/1.1 200 OK" << "\r\n"
  << "Content-Type: text/html"
  << "\r\n" << "Content-Length: 11"
  << "\r\n\r\n"
  << "Hello world";

  sock.SendMessage(std::make_shared<ISocket::Message>(ISocket::Message(response.str().begin(), response.str().end())), client_sock.value());

  return 0;
}
