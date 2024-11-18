#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "Logger.h"

namespace simple_http_server {

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(Logger, MainTest) {
  const std::vector<std::pair<Level, std::string>> levels = {
      {DEBUG, "debug"}, {INFO, "info"}, {WARNING, "warning"}, {ERROR, "error"}};

  Logger::SetLevel(DEBUG);

  for (const auto& line : levels) {
    LOG(line.first, NAMED_OUTPUT(line.second));
  }

  Logger::Flush();

  std::ifstream log(Logger::logFilename);

  std::string time;
  std::string level;
  std::string messageKey;
  std::string messageValue;

  for (const auto& line : levels) {
    log >> time >> level >> messageKey >> messageValue;
    EXPECT_EQ(level, Logger::level_name[line.first]);
    EXPECT_EQ(messageKey, "line.second:");
    EXPECT_EQ(messageValue, line.second);
  }
}
}  // namespace simple_http_server
