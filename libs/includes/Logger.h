#ifndef SIMPLE_HTTP_SERVER_LIBS_LOGGER_H_
#define SIMPLE_HTTP_SERVER_LIBS_LOGGER_H_

#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>

namespace simple_http_server {

#define LOGGER_LEVELS   \
  X(DEBUG, "DEBUG")     \
  X(INFO, "INFO")       \
  X(WARNING, "WARNING") \
  X(ERROR, "ERROR")

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define X(level, name) level,
enum Level : std::uint8_t { LOGGER_LEVELS };
#undef X

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOG(level, message)                \
  Logger::Log((Logger::level_name[level]), \
              (std::ostringstream()        \
               << message))  // NOLINT(bugprone-macro-parentheses)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NAMED_OUTPUT(variable) #variable << ": " << variable

class Logger {
 public:
  static constexpr auto logFilename = "server.log";  // FIXME(dzen) rewrite this

  static void Log(const char* level, std::ostringstream&& message);
  static void Flush();

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define X(level, name) name,
  inline static const std::vector<char const*> level_name = {LOGGER_LEVELS};
#undef X
 private:
  static std::ofstream logStream_;  // NOLINT
};

}  // namespace simple_http_server

#endif  // SIMPLE_HTTP_SERVER_LIBS_LOGGER_H_
