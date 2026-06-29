#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace common {
class Logger {
private:
  std::shared_ptr<spdlog::logger> spdLogger;

  Logger(const std::string &loggerName, const std::string &logDirPath,
         const std::string &appName, spdlog::level::level_enum level);

  static std::unique_ptr<Logger> ptr;
  static bool initialized;

public:
  Logger();
  Logger(Logger const &) = delete;
  Logger(Logger &&) noexcept = delete;
  Logger &operator=(Logger const &) = delete;
  Logger &operator=(Logger &&) noexcept = delete;

  static const Logger &instance();

  static void init(const std::string &loggerName, const std::string &logDirPath,
                   const std::string &appName, spdlog::level::level_enum level);

  static void initNull();

  static bool isInitialized();

  std::shared_ptr<spdlog::logger> getInner() const;
};
} // namespace common
