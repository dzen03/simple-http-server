#include "Server.h"

#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <ios>
#include <iterator>
#include <memory>
#include <regex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>

#include "Directory.h"
#include "ISocket.h"
#include "Logger.h"
#include "Request.h"
#include "Response.h"
#include "SocketFactory.h"
#include "ThreadPool.h"
#include "Util.h"

namespace simple_http_server {

Server::Server(const std::string& ip_addr, int port, Level logLevel)
    : socket_(SocketFactory::CreateSocket(ip_addr, port)) {
  Logger::SetLevel(logLevel);
}

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

auto Server::TryRenderFile(const Request& request) -> Response {
  Response response;
  const auto& first_slash = request.GetUrl().find('/', 1);

  const auto& url_start = request.GetUrl().substr(0, first_slash);

  if (first_slash != std::string::npos &&
      mappedDirectories_.contains(url_start)) {
    auto dir = mappedDirectories_.at(url_start);
    auto path = dir.GetPath();

    std::string::size_type pos = 0;
    auto prev_pos = first_slash;
    while ((pos = request.GetUrl().find('/', prev_pos + 1)) !=
           std::string::npos) {
      path /= request.GetUrl().substr(prev_pos + 1, pos - prev_pos);
      prev_pos = pos;
    }

    path /= request.GetUrl().substr(prev_pos + 1);

    auto check_path = [](const auto& type, const auto& set, const auto& path) {
      bool allowed = (type == Directory::BLACKLIST);
      for (const auto& reg : set) {
        if (std::regex_match(path.string(), reg)) {
          allowed = (type == Directory::WHITELIST);
        }
      }
      return allowed;
    };

    bool allowed =
        check_path(Directory::BLACKLIST, Directory::GetForcedBlacklist(), path);

    allowed =
        (allowed ? check_path(dir.GetType(), dir.GetAllowSet(), path) : false);

    if (!allowed) {
      response = Response(Response::HttpStatusCodes::FORBIDDEN);
    } else if (std::filesystem::exists(path)) {
      response = Render(path, "", dir.GetHeaders());
    }
  }
  return response;
}

auto Server::TryMappedUrls(const Request& request) -> Response {
  Response response;

  const auto& url = request.GetUrl();
  auto current_slash = url.find('/');

  while (current_slash != std::string::npos) {
    current_slash = url.find('/', current_slash + 1);
    const auto& sub_url = url.substr(0, current_slash);

    if (mappedUrls_.contains(sub_url)) {
      const auto& url = mappedUrls_.at(sub_url);

      if (url.second /*include_children*/ ||
          current_slash == std::string::npos) {
        // if allowed subdirs or full path used
        response = url.first(request);
        break;
      }
    }
  }

  return response;
}

void Server::HandleClient(const ISocket::SocketDescriptor& client_sock) {
  const auto& received = socket_->ReceiveMessage(client_sock);

  try {
    const auto& string_request =
        std::string(received->begin(), received->end());

    LOG(DEBUG, "got request " << NAMED_OUTPUT(string_request));

    const Request request(string_request);

    Response response;

    response = TryMappedUrls(request);

    if (response.Empty()) {
      response = TryRenderFile(request);
    }

    if (response.Empty()) {
      response = Response(Response::HttpStatusCodes::NOT_FOUND);
    }

    const auto& string_response = response.Dump();

    socket_->SendMessageAndCloseClient(
        std::make_unique<ISocket::Message>(
            ISocket::Message(string_response.begin(), string_response.end())),
        client_sock);
  } catch (const std::exception& exception) {
    LOG(ERROR, exception.what());

    try {
      auto response = Response(Response::HttpStatusCodes::INTERNAL_ERROR);

      const auto& string_response = response.Dump();

      socket_->SendMessageAndCloseClient(
          std::make_unique<ISocket::Message>(
              ISocket::Message(string_response.begin(), string_response.end())),
          client_sock);
    } catch (const std::exception& exception) {
      LOG(ERROR, exception.what());
    }
  }
}

void Server::Stop() { running_ = false; }

void Server::MapUrl(const std::string& path,
                    const std::function<Response(Request)>& function,
                    bool include_children) {
  if (mappedUrls_.contains(path)) {
    LOG(WARNING, "remapping url " << NAMED_OUTPUT(path));
  }
  LOG(INFO, "mapping URL " << NAMED_OUTPUT(path) << " "
                           << NAMED_OUTPUT(include_children));

  mappedUrls_.emplace(path, std::make_pair(function, include_children));
}

void Server::MapDirectory(const std::string& url, const Directory& directory) {
  if (mappedUrls_.contains(url)) {
    LOG(WARNING, "remapping directory " << NAMED_OUTPUT(url));
  }
  LOG(INFO, "mapping directory " << url << " --> " << directory.GetPath());

  mappedDirectories_.emplace(url, directory);
}

auto Server::Render(const std::filesystem::path& file,
                    const std::string& content_type,
                    HeadersMap headers) -> Response {
  std::ifstream temp(file, std::ios::binary);

  std::string content((std::istreambuf_iterator<char>(temp)),
                      std::istreambuf_iterator<char>());

  if (!headers.contains("Content-Type")) {
    headers.emplace("Content-Type",
                    content_type.empty()
                        ? DeduceContentType(file) + "; charset = utf-8"
                        : content_type);
  }

  return Response(Response::HttpStatusCodes::OK, std::move(content), headers);
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
