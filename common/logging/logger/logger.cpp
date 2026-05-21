#include "logger.h"

#include <filesystem>
#include <iostream>
#include <spdlog/sinks/hourly_file_sink.h>
#include <spdlog/sinks/null_sink.h>

namespace common {
std::unique_ptr<Logger> Logger::ptr = nullptr;
bool Logger::initialized = false;

Logger::Logger() { spdLogger = spdlog::null_logger_mt("null logger"); }

Logger::Logger(const std::string &loggerName, const std::string &logDirPath,
               const std::string &appName, spdlog::level::level_enum level) {
  std::filesystem::create_directories(logDirPath);
  std::string logFilePath = std::filesystem::path(logDirPath) / appName;

  spdLogger = spdlog::hourly_logger_mt(loggerName, logFilePath);
  spdLogger->set_level(level);
  spdLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] [%s:%!] %v");
  spdLogger->set_error_handler([](const std::string &msg) {
    std::cerr << "Logging error: " << msg << std::endl;
  });
}

void Logger::init(const std::string &loggerName, const std::string &logDirPath,
                  const std::string &appName, spdlog::level::level_enum level) {
  if (!initialized) {
    ptr = std::unique_ptr<Logger>(
        new Logger(loggerName, logDirPath, appName, level));
    initialized = true;
  }
}

void Logger::disable() {
  if (!initialized) {
    ptr = std::unique_ptr<Logger>(new Logger());
    initialized = true;
  }
}

const Logger &Logger::instance() {
  if (!ptr) {
    throw std::runtime_error("Logger not initialized");
  }
  return *ptr;
}

bool Logger::isInitialized() { return initialized; }

std::shared_ptr<spdlog::logger> Logger::getInner() const { return spdLogger; }

} // namespace common
