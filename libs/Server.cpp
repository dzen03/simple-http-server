#include "Logger.h"
#include "Request.h"
#include "Response.h"
#include "Server.h"
#include "Util.h"

#include "SocketFactory.h"

#include <sstream>
#include <string>
#include <unordered_map>

namespace simple_http_server {

Server::Server() {
  socket_ = SocketFactory::CreateSocket();
}

void Server::Start(const std::string &ip_addr, int port) {
  if (mappedUrls_.empty()) {
    LOG(WARNING, "there is no urls mapped");
  }

  running_ = true;
  socket_->BindAndListen(ip_addr, port);

  while (running_) {
    try {
      const auto &client_sock = socket_->Accept();

      const auto &received = socket_->ReceiveMessage(client_sock);

      const auto &string_request = std::string(received->begin(), received->end());

      LOG(INFO, "got request " << NAMED_OUTPUT(string_request));

      Request request(string_request);

      Response response;

      if (mappedUrls_.contains(request.GetUrl())) {
        response = mappedUrls_[request.GetUrl()](request);
      } else {
        const auto &first_slash = request.GetUrl().find('/', 1);

        const auto &url_start = request.GetUrl().substr(0, first_slash);

        if (first_slash != std::string::npos && mappedDirectories_.contains(url_start)) {

          auto path = mappedDirectories_.at(url_start);

          size_t pos = 0;
          size_t prev_pos = first_slash;
          while ((pos = request.GetUrl().find('/', prev_pos + 1)) != std::string::npos) {
            path /= request.GetUrl().substr(prev_pos + 1, pos - prev_pos);
            prev_pos = pos;
          }

          path /= request.GetUrl().substr(prev_pos + 1);

          if (std::filesystem::exists(path)) {
            response = Render(path);
          }
        }
      }

      if (response.Empty()) {
        static constexpr int NOT_FOUND_CODE = 404;
        response = Response(NOT_FOUND_CODE);
      }

      const auto &string_response = response.Dump();

      socket_->SendMessageAndCloseClient(std::make_unique<ISocket::Message>(
                                             ISocket::Message(string_response.begin(), string_response.end())),
                                         client_sock);
    }
    catch (const std::runtime_error &exception) {
      LOG(ERROR, exception.what());
    }
  }
}

void Server::Stop() {
  running_ = false;
}

void Server::MapUrl(const std::string &path, const std::function<Response(Request)> &function) {
  if (mappedUrls_.contains(path)) {
    LOG(WARNING, "remapping url " << NAMED_OUTPUT(path));
  }
  LOG(INFO, "mapping URL " << NAMED_OUTPUT(path));

  mappedUrls_.insert({path, function});
}

void Server::MapDirectory(const std::string &url, const std::filesystem::path &directory) {
  LOG(INFO, "mapping directory " << url << " --> " << directory);

  mappedDirectories_.insert({url, directory});
}

auto Server::Render(const std::filesystem::path &file, const std::string &content_type) -> Response {
  const std::ifstream temp(file);
  std::stringstream buffer;
  buffer << temp.rdbuf(); // redundant copy

  static constexpr int OK_CODE = 200;
  return Response(OK_CODE, buffer.str(), {{"Content-Type", content_type.empty() ? DeduceContentType(file) + "; charset = utf-8" : content_type}});
}

auto Server::DeduceContentType(const std::filesystem::path &path) -> std::string {
  const auto &ext = path.extension();

  static const std::unordered_map<std::string, std::string> ext_to_mimo {
      {".html", "text/html"},
      // TODO(all) add your own types here
  };

  if (ext_to_mimo.contains(ext.string())) {
    return ext_to_mimo.at(ext.string());
  }

  // default as of https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
  return "application/octet-stream";
}


} // namespace simple_http_server
