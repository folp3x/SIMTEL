#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace common {
class Logger {
private:
  static std::shared_ptr<spdlog::logger> spdLogger;

protected:
  static void setLoggerOptions();

public:
  static void initLogging(const std::string &loggerName,
                          const std::string &logDirPath,
                          const std::string &appName);
};
} // namespace common
