#pragma once

#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace common {
class Logger {
private:
  static std::shared_ptr<spdlog::logger> spdLogger;

protected:
  static void setLoggerOptions();

public:
  static void initLogging(const std::string &loggerName,
                          const std::string &logFilePath);
};
} // namespace common
