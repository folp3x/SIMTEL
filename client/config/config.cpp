#include "config.h"

namespace client {
bool Config::isInitialized() const {
  return common::Config::isInitialized() && imei != "" && imsi != "";
}

std::string Config::getImei() const { return imei; }

void Config::setImei(const std::string &imei_) { imei = imei_; }

std::string Config::getImsi() const { return imsi; }

void Config::setImsi(const std::string &imsi_) { imsi = imsi_; }
} // namespace client
