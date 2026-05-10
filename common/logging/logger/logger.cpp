#include "logger.h"

#include <iostream>

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
                         const std::string &logFilePath) {
  // настройка логирования
  spdLogger = spdlog::basic_logger_mt(loggerName, logFilePath);

  setLoggerOptions();

  spdlog::set_default_logger(spdLogger);
}
} // namespace common
