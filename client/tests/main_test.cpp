#include <gtest/gtest.h>

#include "common/logging/logger/logger.h"

int main(int argc, char **argv) {
  // отключение логирования
  common::Logger::initNull();

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
