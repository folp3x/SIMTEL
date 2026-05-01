#ifndef CONFIG_H
#define CONFIG_H

#include <array>
#include <string>

#include "common/constants.h"

struct Config {
  std::string ip = "";
  int port = -1;
  std::string imei = "unknown";
  std::string imsi = "unknown";
  std::array<double, Constants::LOCATION_COORDS_COUNT> loc = {0, 0, 0};
};

#endif // CONFIG_H
