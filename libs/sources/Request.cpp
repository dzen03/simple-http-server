#include "Request.h"

#include <sstream>

#include "Logger.h"
#include "Util.h"

namespace simple_http_server {

auto Request::ParseArguments(const std::string& url_with_args) -> ArgumentsMap {
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
      ret.emplace(key, value);
    }

    return next_arg;
  };

  while (next_arg != std::string::npos) {
    next_arg = parse_argument(next_arg);
  }

  return ret;
}

Request::Request(const std::string& stringRequest) {
  std::istringstream req(stringRequest);

  std::string type;
  req >> type;

  if (type == "GET") {
    type_ = Request::Type::GET;
  } else if (type == "POST") {
    type_ = Request::Type::POST;
  } else {
    LOG(WARNING, "got unknown request type: " << stringRequest);
    type_ = Request::Type::UNKNOWN;
  }

  std::string url_with_args;

  req >> url_with_args >> httpVersion_;

  url_ = url_with_args.substr(0, url_with_args.find('?'));
  arguments_ = ParseArguments(url_with_args);

  std::string header_line;

  req.ignore(2, '\n');  // skip \r\n after "operator>>" (because they stay)

  while (std::getline(req, header_line, '\n')) {
    if (header_line.ends_with('\r')) {
      header_line.pop_back();
    }

    if (header_line.empty()) {
      break;
    }

    auto colon = header_line.find(':');

    const auto value_start_pos =
        header_line.find_first_not_of(" \n\r\t", colon + 1);

    if (colon == std::string::npos || value_start_pos == std::string::npos) {
      LOG(WARNING, "got incorrect header " << NAMED_OUTPUT(header_line));
      continue;
    }

    const auto& key = header_line.substr(0, colon);

    const auto& value = header_line.substr(value_start_pos);

    headers_.emplace(key, value);
  }

  req >> body_;
}

}  // namespace simple_http_server
