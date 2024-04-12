#include "Server.h"
#include "Logger.h"

#include <sstream>
#include <string>

namespace simple_http_server {
void Server::Start() {

}

Server::Request Server::ParseRequest(const std::string& stringRequest) {
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

  req >> ret.url >> ret.httpVersion;

  std::string header_line;

  req.ignore(2, '\n'); // skip /r/n after >> (because they stay)

  while(std::getline(req, header_line, '\n')){
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

Server::Response Server::CreateResponse(int statusCode, const std::string& body, const std::string& statusMessage,
                                        const HeadersMap& headers) {
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

std::string Server::DumpResponse(const Server::Response& response) {
  std::ostringstream string_response;

  string_response << response.httpVersion << " " << response.statusCode << " " << response.statusMessage << "\r\n";

  for (const auto& header : response.headers) {
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
