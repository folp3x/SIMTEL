#include "logger.h"

#include <filesystem>
#include <iostream>
#include <unistd.h>

#include <spdlog/sinks/hourly_file_sink.h>

namespace common {
std::shared_ptr<spdlog::logger> Logger::spdLogger = nullptr;

void Logger::setLoggerOptions() {
  spdLogger->set_level(spdlog::level::info);
  // формат логов: дата, время, уровень, файл, функция, сообщение
  spdLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] [%s:%!] %v");
  spdLogger->set_error_handler([](const std::string &msg) {
    std::cerr << "Logging error: " << msg << std::endl;
  });
}

void Logger::initLogging(const std::string &loggerName,
                         const std::string &logDirPath,
                         const std::string &appName) {
  // настройка логирования
  int pid = getpid();
  std::string logFileName = appName + "_pid" + std::to_string(pid);
  std::string logFilePath = std::filesystem::path(logDirPath) / logFileName;
  spdLogger = spdlog::hourly_logger_mt(loggerName, logFilePath);
  setLoggerOptions();
  spdlog::set_default_logger(spdLogger);
}
} // namespace common
