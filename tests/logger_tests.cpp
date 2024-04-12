#include "util.h"

#include "Logger.h"
#include <fstream>
#include <gtest/gtest.h>

using namespace simple_http_server;


TEST(Logger, MainTest) {
  const std::vector<std::pair<Level, std::string>> levels = {{DEBUG, "debug"}, {INFO, "info"},
                                                             {WARNING, "warning"}, {ERROR, "error"}};


  for (const auto& line : levels) {
    LOG(line.first, NAMED_OUTPUT(line.second));
  }

  std::ifstream log(Logger::logFilename);

  std::string time, level, messageKey, messageValue;
  for (const auto& line : levels) {
    log >> time >> level >> messageKey >> messageValue;
    EXPECT_EQ(level, Logger::level_name[line.first]);
    EXPECT_EQ(messageKey, "line.second:");
    EXPECT_EQ(messageValue, line.second);
  }
}
