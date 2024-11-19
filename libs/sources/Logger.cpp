#include "Logger.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>

namespace simple_http_server {

std::filesystem::path Logger::logFilename_ = "./server.log";
std::ofstream Logger::logStream_;
Level Logger::logLevel_ = WARNING;
bool Logger::isInit_ = false;

void Logger::Init() {
  // TODO(dzen) think about mutex here
  if (isInit_) {
    return;
  }

  Logger::logStream_ = std::ofstream(logFilename_);
  Logger::isInit_ = true;
}

void Logger::Flush() {
  Init();
  Logger::logStream_.flush();
}
void Logger::SetLevel(Level newLevel) { Logger::logLevel_ = newLevel; }
void Logger::SetFilename(const std::filesystem::path& newFilename) {
  Logger::logFilename_ = newFilename;
  Logger::logStream_.close();
  Init();
}
void Logger::Log(Level level, std::ostringstream&& message) {
  Init();
  if (level < logLevel_) {
    return;
  }

  const time_t now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  Logger::logStream_ << std::put_time(std::localtime(&now), "%Y-%m-%d@%H:%M:%S")
                     << " " << level_name[level] << " "
                     << std::move(message).str() << "\n";
  Flush();
  // TODO(dzen) think about flush(); probably should use Flush() each N lines
  // TODO(dzen) add log rotation
}

}  // namespace simple_http_server
