#include "Server.h"

#include <sstream>
#include <string>
#include <unordered_map>

#include "ISocket.h"
#include "Logger.h"
#include "Request.h"
#include "Response.h"
#include "SocketFactory.h"
#include "ThreadPool.h"
#include "Util.h"

namespace simple_http_server {

Server::Server(const std::string& ip_addr, int port)
    : socket_(SocketFactory::CreateSocket(ip_addr, port)) {}

void Server::Start() {
  if (mappedUrls_.empty()) {
    LOG(WARNING, "there is no urls mapped");
  }

  LOG(INFO, "Using " << std::thread::hardware_concurrency() << " threads");
  ThreadPool threadPool(std::thread::hardware_concurrency());

  running_ = true;
  socket_->BindAndListen();

  while (running_) {
    try {
      const auto& client_sock = socket_->Accept();

      threadPool.enqueue([this, client_sock]() { HandleClient(client_sock); });

    } catch (const std::exception& exception) {
      LOG(ERROR, exception.what());
    }
  }
}

void Server::HandleClient(const ISocket::SocketDescriptor& client_sock) {
  const auto& received = socket_->ReceiveMessage(client_sock);

  try {
    const auto& string_request =
        std::string(received->begin(), received->end());

    LOG(INFO, "got request " << NAMED_OUTPUT(string_request));

    const Request request(string_request);

    Response response;

    if (mappedUrls_.contains(request.GetUrl())) {
      response = mappedUrls_[request.GetUrl()](request);
    } else {
      const auto& first_slash = request.GetUrl().find('/', 1);

      const auto& url_start = request.GetUrl().substr(0, first_slash);

      if (first_slash != std::string::npos &&
          mappedDirectories_.contains(url_start)) {
        auto path = mappedDirectories_.at(url_start);

        size_t pos = 0;
        size_t prev_pos = first_slash;
        while ((pos = request.GetUrl().find('/', prev_pos + 1)) !=
               std::string::npos) {
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

    const auto& string_response = response.Dump();

    socket_->SendMessageAndCloseClient(
        std::make_unique<ISocket::Message>(
            ISocket::Message(string_response.begin(), string_response.end())),
        client_sock);
  } catch (const std::exception& exception) {
    LOG(ERROR, exception.what());
  }
}

void Server::Stop() { running_ = false; }

void Server::MapUrl(const std::string& path,
                    const std::function<Response(Request)>& function) {
  if (mappedUrls_.contains(path)) {
    LOG(WARNING, "remapping url " << NAMED_OUTPUT(path));
  }
  LOG(INFO, "mapping URL " << NAMED_OUTPUT(path));

  mappedUrls_.emplace(path, function);
}

void Server::MapDirectory(const std::string& url,
                          const std::filesystem::path& directory) {
  LOG(INFO, "mapping directory " << url << " --> " << directory);

  mappedDirectories_.emplace(url, directory);
}

auto Server::Render(const std::filesystem::path& file,
                    const std::string& content_type) -> Response {
  const std::ifstream temp(file);
  std::stringstream buffer;
  buffer << temp.rdbuf();  // FIXME(dzen) redundant copy

  static constexpr int OK_CODE = 200;
  return Response(
      OK_CODE, buffer.str(),
      {{"Content-Type", content_type.empty()
                            ? DeduceContentType(file) + "; charset = utf-8"
                            : content_type}});
}

auto Server::DeduceContentType(const std::filesystem::path& path)
    -> std::string {
  const auto& ext = path.extension();

  static const std::unordered_map<std::string, std::string> ext_to_mime{
      // got from
      // https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
      {".aac", "audio/aac"},
      {".abw", "application/x-abiword"},
      {".apng", "image/apng"},
      {".arc", "application/x-freearc"},
      {".avif", "image/avif"},
      {".avi", "video/x-msvideo"},
      {".azw", "application/vnd.amazon.ebook"},
      {".bin", "application/octet-stream"},
      {".bmp", "image/bmp"},
      {".bz", "application/x-bzip"},
      {".bz2", "application/x-bzip2"},
      {".cda", "application/x-cdf"},
      {".csh", "application/x-csh"},
      {".css", "text/css"},
      {".csv", "text/csv"},
      {".doc", "application/msword"},
      {".docx",
       "application/"
       "vnd.openxmlformats-officedocument.wordprocessingml.document"},
      {".eot", "application/vnd.ms-fontobject"},
      {".epub", "application/epub+zip"},
      {".gz", "application/gzip"},
      {".gif", "image/gif"},
      {".htm", "text/html"},
      {".html", "text/html"},
      {".ico", "image/vnd.microsoft.icon"},
      {".ics", "text/calendar"},
      {".jar", "application/java-archive"},
      {".jpg", "image/jpeg"},
      {".jpeg", "image/jpeg"},
      {".js", "text/javascript"},
      {".json", "application/json"},
      {".jsonld", "application/ld+json"},
      {".mid", "audio/midi"},
      {".midi", "audio/midi"},
      {".mjs", "text/javascript"},
      {".mp3", "audio/mpeg"},
      {".mp4", "video/mp4"},
      {".mpeg", "video/mpeg"},
      {".mpkg", "application/vnd.apple.installer+xml"},
      {".odp", "application/vnd.oasis.opendocument.presentation"},
      {".ods", "application/vnd.oasis.opendocument.spreadsheet"},
      {".odt", "application/vnd.oasis.opendocument.text"},
      {".oga", "audio/ogg"},
      {".ogv", "video/ogg"},
      {".ogx", "application/ogg"},
      {".opus", "audio/ogg"},
      {".otf", "font/otf"},
      {".png", "image/png"},
      {".pdf", "application/pdf"},
      {".php", "application/x-httpd-php"},
      {".ppt", "application/vnd.ms-powerpoint"},
      {".pptx",
       "application/"
       "vnd.openxmlformats-officedocument.presentationml.presentation"},
      {".rar", "application/vnd.rar"},
      {".rtf", "application/rtf"},
      {".sh", "application/x-sh"},
      {".svg", "image/svg+xml"},
      {".tar", "application/x-tar"},
      {".tif", "image/tiff"},
      {".tiff", "image/tiff"},
      {".ts", "video/mp2t"},
      {".ttf", "font/ttf"},
      {".txt", "text/plain"},
      {".vsd", "application/vnd.visio"},
      {".wav", "audio/wav"},
      {".weba", "audio/webm"},
      {".webm", "video/webm"},
      {".webp", "image/webp"},
      {".woff", "font/woff"},
      {".woff2", "font/woff2"},
      {".xhtml", "application/xhtml+xml"},
      {".xls", "application/vnd.ms-excel"},
      {".xlsx",
       "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
      {".xml", "application/xml"},
      {".xul", "application/vnd.mozilla.xul+xml"},
      {".zip", "application/zip"},
      {".3gp", "video/3gpp"},
      {".3g2", "video/3gpp2"},
      {".7z", "application/x-7z-compressed"},
      // TODO(all) add your own types here
  };

  if (ext_to_mime.contains(ext.string())) {
    return ext_to_mime.at(ext.string());
  }

  // default as of
  // https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
  return "application/octet-stream";
}

}  // namespace simple_http_server
