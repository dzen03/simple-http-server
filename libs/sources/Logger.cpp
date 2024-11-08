#include "Logger.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>

namespace simple_http_server {

// FIXME(dzen) rewrite this
// NOLINTNEXTLINE
std::ofstream Logger::logStream_ = std::ofstream(logFilename);

void Logger::Log(const char* level, std::ostringstream&& message) {
  const time_t now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  logStream_ << std::put_time(std::localtime(&now), "%Y-%m-%d@%H:%M:%S") << " "
             << level << " " << std::move(message).str() << "\n";
  Flush();
  // TODO(dzen) think about flush(); probably should use Flush() each N lines
  // TODO(dzen) add log rotation
}

void Logger::Flush() { logStream_.flush(); }

}  // namespace simple_http_server
