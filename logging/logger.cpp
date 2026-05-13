#include "logger.h"

#include <iostream>

std::shared_ptr<spdlog::logger> Logger::spdLogger = nullptr;

void Logger::initLogging() {
  // настройка логирования
  auto spdLogger = spdlog::basic_logger_mt("app_logger", "logs/app_log.txt");
  spdLogger->set_level(spdlog::level::info);
  // формат логов: дата, время, уровень, файл, функция, сообщение
  spdLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] [%s:%!] %v");
  spdLogger->set_error_handler([](const std::string &msg) {
    std::cerr << "Logging error: " << msg << std::endl;
  });

  spdlog::set_default_logger(spdLogger);
}
