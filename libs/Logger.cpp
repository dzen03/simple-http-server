#include "Logger.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace simple_http_server {

std::ofstream Logger::logStream_ = std::ofstream(logFilename);

void Logger::Log(const char* level, std::ostringstream&& message) {
  const time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  logStream_ << std::put_time(std::localtime(&now), "%F@%T") << " "
             << level << " "
             << message.str() << std::endl;
}

} // simple_http_server
