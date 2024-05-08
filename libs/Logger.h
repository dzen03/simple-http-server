#ifndef SIMPLE_HTTP_SERVER_LIBS_LOGGER_H_
#define SIMPLE_HTTP_SERVER_LIBS_LOGGER_H_

#include <fstream>

namespace simple_http_server {

#define LOGGER_LEVELS \
  X(DEBUG, "DEBUG") \
  X(INFO, "INFO") \
  X(WARNING, "WARNING") \
  X(ERROR, "ERROR") \

#define X(level, name) level,
enum Level {
  LOGGER_LEVELS
};
#undef X

#define LOG(level, message) Logger::Log((Logger::level_name[level]), (std::ostringstream() << message))

#define NAMED_OUTPUT(variable) #variable << ": " << variable

class Logger {
 public:
  static constexpr auto logFilename = "server.log";

  Logger() = default;
  static void Log(const char *level, std::ostringstream &&message);

#define X(level, name) name,
  inline static char const *level_name[] =
      {
          LOGGER_LEVELS
      };
#undef X
 private:
  static std::ofstream logStream_;
};

} // simple_http_server

#endif //SIMPLE_HTTP_SERVER_LIBS_LOGGER_H_
