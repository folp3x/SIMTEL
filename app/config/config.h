#ifndef CONFIG_H
#define CONFIG_H

#include <array>
#include <string>

struct Config {
  static const int COORDS_COUNT = 3;

  std::string ip = "";
  int port = -1;
  std::string imei = "unknown";
  std::string imsi = "unknown";
  std::array<double, COORDS_COUNT> loc = {0, 0, 0};
};

#endif // CONFIG_H
