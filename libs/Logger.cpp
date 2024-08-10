#include "Logger.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace simple_http_server {

std::ofstream Logger::logStream_ = std::ofstream(logFilename);

void Logger::Log(const char *level, std::ostringstream &&message) {
  const time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  logStream_ << std::put_time(std::localtime(&now), "%Y-%m-%d@%H:%M:%S") << " "
             << level << " "
             << message.str() << "\n";
  Flush();
  // TODO(dzen) think about flush()
}

void Logger::Flush() {
  logStream_.flush();
}

} // namespace simple_http_server
