#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace common {
class Logger {
private:
  std::shared_ptr<spdlog::logger> spdLogger;

  static std::unique_ptr<Logger> ptr;
  static std::once_flag initialized;

  Logger(const std::string &loggerName, const std::string &logDirPath,
         const std::string &appName, spdlog::level::level_enum level);

  Logger();
  Logger(Logger const &) = delete;
  Logger(Logger &&) noexcept = delete;
  Logger &operator=(Logger const &) = delete;
  Logger &operator=(Logger &&) noexcept = delete;

public:
  static const Logger &instance();

  static void init(const std::string &loggerName, const std::string &logDirPath,
                   const std::string &appName, spdlog::level::level_enum level);

  static void disable();

  std::shared_ptr<spdlog::logger> getInner() const;
};
} // namespace common
