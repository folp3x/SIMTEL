#include "config.h"

#include "common/validator/validator.h"

namespace client {
std::unique_ptr<common::BaseJsonInfo> Config::getJsonRootInfo() {
  auto root = common::Config::getJsonRootInfo();
  auto *rootPtr = dynamic_cast<common::JsonObjectInfo *>(root.get());

  rootPtr->addInner("imei", makeJsonValue<common::imei_t>(
                                &imei, common::Validator::isCorrectImei));

  rootPtr->addInner("imsi", makeJsonValue<common::imsi_t>(
                                &imsi, common::Validator::isCorrectImsi));

  rootPtr->addInner(
      "ip", makeJsonValue<std::string>(&ip, common::Validator::isCorrectIpStr));

  rootPtr->addInner("addressBookFilePath",
                    makeJsonValue<std::string>(
                        &addressBookFilePath, [](const std::string &filePath) {
                          return common::Validator::jsonFilePathExists(
                              filePath, "Address book");
                        }));

  rootPtr->addInner("loc", makeJsonArray(&loc));

  return root;
}

std::string Config::getImei() const { return imei; }

void Config::setImei(const common::imei_t &imei_) { imei = imei_; }

std::string Config::getImsi() const { return imsi; }

void Config::setImsi(const common::imsi_t &imsi_) { imsi = imsi_; }

std::string Config::getIp() const { return ip; }

void Config::setIp(const std::string &ip_) { ip = ip_; }

std::string Config::getAddressBookFilePath() const {
  return addressBookFilePath;
}
common::coords_t<> Config::getLoc() const { return loc; }

void Config::setLoc(const common::coords_t<> &loc_) { loc = loc_; }
} // namespace client
