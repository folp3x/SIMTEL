#include "logger.h"

#include <filesystem>
#include <iostream>

#include <spdlog/sinks/hourly_file_sink.h>

namespace common {
std::shared_ptr<spdlog::logger> Logger::spdLogger = nullptr;

void Logger::initLogging(const std::string &loggerName,
                         const std::string &logDirPath,
                         const std::string &appName,
                         spdlog::level::level_enum level) {
  std::filesystem::create_directories(logDirPath);

  std::string logFilePath = std::filesystem::path(logDirPath) / appName;
  spdLogger = spdlog::hourly_logger_mt(loggerName, logFilePath);

  spdLogger->set_level(level);
  spdLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] [%s:%!] %v");
  spdLogger->set_error_handler([](const std::string &msg) {
    std::cerr << "Logging error: " << msg << std::endl;
  });

  spdlog::set_default_logger(spdLogger);
}
} // namespace common
