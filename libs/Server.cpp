#include "Logger.h"
#include "Server.h"

#include "SocketFactory.h"

#include <sstream>
#include <string>

namespace simple_http_server {

Server::Server() {
  socket_ = SocketFactory::CreateSocket();
}

void Server::Start(const std::string &ip, int port) {
  if (mappedUrls_.empty()) {
    LOG(WARNING, "there is no urls mapped");
  }

  running_ = true;
  socket_->BindAndListen(ip, port);

  auto buffer = std::make_shared<ISocket::Message>(ISocket::Message(1000));

  while (running_) {
    auto client_sock = socket_->Accept();

    if (!client_sock) {
      continue;
    }

    int received_len = socket_->ReceiveMessage(client_sock.value(), buffer);

    std::string string_request = std::string(buffer->begin(), buffer->end());
    string_request.resize(received_len);

    LOG(INFO, "got request " << NAMED_OUTPUT(string_request));

    Request request = ParseRequest(string_request);

    Response response;
    if (mappedUrls_.contains(request.url)) {
      response = mappedUrls_[request.url](request);
    } else {
      response = CreateResponse(404);
    }

    auto string_response = DumpResponse(response);

    socket_->SendMessageAndCloseClient(std::make_shared<ISocket::Message>(
                                           ISocket::Message(string_response.begin(), string_response.end())),
                                       client_sock.value());
  }
}

void Server::Stop() {
  running_ = false;
}

void Server::MapUrl(const std::string &path, const std::function<Server::Response(Server::Request)> &function) {
  if (mappedUrls_.contains(path)) {
    LOG(WARNING, "remapping url " << NAMED_OUTPUT(path));
  }
  LOG(INFO, "mounting " << NAMED_OUTPUT(path));

  mappedUrls_.insert({path, function});
}

Server::ArgumentsMap Server::ParseArguments(const std::string &url_with_args) {
  auto args_start = url_with_args.find('?');

  ArgumentsMap ret;

  auto next_arg = args_start;

  auto parse_argument = [&ret, &url_with_args](auto current_arg) mutable {
    auto sep = url_with_args.find('=', current_arg);
    auto next_arg = url_with_args.find('&', current_arg + 1);

    if (sep == std::string::npos) {
      LOG(WARNING, "incorrect arguments " << NAMED_OUTPUT(url_with_args));
    } else {
      auto key = url_with_args.substr(current_arg + 1, sep - current_arg - 1);
      auto value = url_with_args.substr(sep + 1, next_arg - sep - 1);
      ret.insert({key, value});
    }

    return next_arg;
  };

  while (next_arg != std::string::npos) {
    next_arg = parse_argument(next_arg);
  }

  return ret;
}

Server::Request Server::ParseRequest(const std::string &stringRequest) {
  std::istringstream req(stringRequest);

  Request ret;

  std::string type;
  req >> type;

  if (type == "GET") ret.type = Request::Type::GET;
  else if (type == "POST") ret.type = Request::Type::POST;
  else {
    LOG(WARNING, "got unknown request type: " << stringRequest);
    ret.type = Request::Type::UNKNOWN;
  }

  std::string url_with_args;

  req >> url_with_args >> ret.httpVersion;

  ret.url = url_with_args.substr(0, url_with_args.find('?'));
  ret.arguments = ParseArguments(url_with_args);

  std::string header_line;

  req.ignore(2, '\n'); // skip /r/n after "operator>>" (because they stay)

  while (std::getline(req, header_line, '\n')) {
    if (header_line.ends_with('\r')) {
      header_line.pop_back();
    }

    if (header_line.empty()) {
      break;
    }

    auto colon = header_line.find(':');

    if (colon == std::string::npos) {
      LOG(WARNING, "got incorrect header " << NAMED_OUTPUT(header_line));
      continue;
    }

    std::string key = header_line.substr(0, colon);

    std::string value = header_line.substr(header_line.find_first_not_of(" \n\r\t", colon + 1));

    ret.headers.insert({key, value});
  }

  req >> ret.body;

  return ret;
}

Server::Response Server::CreateResponse(int statusCode, const std::string &body, const std::string &statusMessage,
                                        const HeadersMap &headers) {
  Response response = {.httpVersion = "HTTP/1.1",
      .statusCode = statusCode,
      .statusMessage = (statusMessage.empty() ? defaultMessages_.at(statusCode) : statusMessage),
      .headers = headers,
      .body = body};

  if (!headers.contains("Content-Length")) {
    response.headers.insert({"Content-Length", std::to_string(response.body.length())});
  }

  return response;
}

std::string Server::DumpResponse(const Server::Response &response) {
  std::ostringstream string_response;

  string_response << response.httpVersion << " " << response.statusCode << " " << response.statusMessage << "\r\n";

  for (const auto &header : response.headers) {
    string_response << header.first << ": " << header.second << "\r\n";
  }

  if (response.headers.empty()) {
    string_response << "\r\n";
  }

  string_response << "\r\n"
                  << response.body;

  return string_response.str();
}

} // simple_http_server
