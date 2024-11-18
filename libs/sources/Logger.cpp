#include "Logger.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>

namespace simple_http_server {

// FIXME(dzen) rewrite this
// NOLINTNEXTLINE
std::ofstream Logger::logStream_ = std::ofstream(logFilename);
// NOLINTNEXTLINE
Level Logger::logLevel_ = WARNING;

void Logger::Log(Level level, std::ostringstream&& message) {
  if (level < logLevel_) {
    return;
  }

  const time_t now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  logStream_ << std::put_time(std::localtime(&now), "%Y-%m-%d@%H:%M:%S") << " "
             << Logger::level_name[level] << " " << std::move(message).str()
             << "\n";
  Flush();
  // TODO(dzen) think about flush(); probably should use Flush() each N lines
  // TODO(dzen) add log rotation
}

void Logger::Flush() { logStream_.flush(); }

void Logger::SetLevel(Level newLevel) { logLevel_ = newLevel; }

}  // namespace simple_http_server
