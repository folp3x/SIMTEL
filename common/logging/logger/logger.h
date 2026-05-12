#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace common {
class Logger {
private:
  static std::shared_ptr<spdlog::logger> spdLogger;

public:
  static void initLogging(const std::string &loggerName,
                          const std::string &logDirPath,
                          const std::string &appName,
                          spdlog::level::level_enum level);
};
} // namespace common
