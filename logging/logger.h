#pragma once

#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

class Logger {
  static std::shared_ptr<spdlog::logger> spdLogger;

public:
  static void initLogging();
};
