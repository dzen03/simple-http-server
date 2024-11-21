#ifndef SIMPLE_HTTP_SERVER_LIBS_LOGGER_H_
#define SIMPLE_HTTP_SERVER_LIBS_LOGGER_H_

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

namespace simple_http_server {

#define LOGGER_LEVELS      \
  X(DEBUG, "DEBUG", 0)     \
  X(INFO, "INFO", 1)       \
  X(WARNING, "WARNING", 2) \
  X(ERROR, "ERROR", 3)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define X(level, name, i) level = (i),
enum Level : std::uint8_t { LOGGER_LEVELS };
#undef X

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOG(level, message)                  \
  Logger::Log((level), (std::ostringstream() \
                        << message))  // NOLINT(bugprone-macro-parentheses)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NAMED_OUTPUT(variable) #variable << ": " << variable

class Logger {
 public:
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define X(level, name, _) name,
  inline static const std::vector<char const*> level_name = {LOGGER_LEVELS};
#undef X
 private:
  static std::filesystem::path logFilename_;
  static std::ofstream logStream_;
  static Level logLevel_;
  static bool isInit_;

 public:
  static void Init();

  static void Flush();
  static void SetLevel(Level newLevel);
  static auto GetFilename() { return Logger::logFilename_; }
  static void SetFilename(const std::filesystem::path& newFilename);
  static void Log(Level level, std::ostringstream&& message);
};

}  // namespace simple_http_server

#endif  // SIMPLE_HTTP_SERVER_LIBS_LOGGER_H_
