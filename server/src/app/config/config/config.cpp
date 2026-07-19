#include "config.h"

#include "common/validator/validator.h"

namespace server {
std::unique_ptr<common::BaseJsonInfo> Config::getJsonRootInfo() {
  auto root = common::Config::getJsonRootInfo();
  auto *rootPtr = dynamic_cast<common::JsonObjectInfo *>(root.get());

  rootPtr->addInner(
      "bsFilePath",
      makeJsonValue<std::string>(&bsFilePath, [](const std::string &path) {
        return common::Validator::jsonFilePathExists(path, "BS");
      }));

  rootPtr->addInner(
      "epcFilePath",
      makeJsonValue<std::string>(&epcFilePath, [](const std::string &path) {
        return common::Validator::jsonFilePathExists(path, "EPC");
      }));

  auto smscConfigObj = makeJsonObject();
  smscConfigObj->addInner(
      "smsTtl_ms",
      makeJsonValue<unsigned int>(&smscConfig.smsTtlMs, [](unsigned int ttl) {
        return common::Validator::isPositiveNumber(ttl, "SMS TTL");
      }));
  rootPtr->addInner("smscConfig", std::move(smscConfigObj));

  auto mmeConfigObj = makeJsonObject();
  mmeConfigObj->addInner(
      "id", makeJsonValue<unsigned int>(&curMmeConfig.id, [](unsigned int id) {
        return common::Validator::isPositiveNumber(id, "MME id");
      }));
  mmeConfigObj->addInner(
      "maxVlrSize",
      makeJsonValue<size_t>(&curMmeConfig.maxVlrSize, [](size_t size) {
        return common::Validator::isPositiveNumber(size, "Max VLR size");
      }));
  rootPtr->addInner("mmeConfigs",
                    makeJsonRepeatObject(std::move(mmeConfigObj), [this]() {
                      mmeConfigs.push_back(curMmeConfig);
                    }));

  auto pcrfConfigObj = makeJsonObject();
  pcrfConfigObj->addInner(
      "smsPrice_rub",
      makeJsonValue<double>(&pcrfConfig.smsPriceRub, [](double price) {
        return common::Validator::isPositiveNumber(price, "SMS price");
      }));

  auto balanceInfoObj = makeJsonObject();
  balanceInfoObj->addInner(
      "imsi", makeJsonValue<common::imsi_t>(&curBalanceInfo.imsi,
                                            common::Validator::isCorrectImsi));
  balanceInfoObj->addInner("balance_rub",
                           makeJsonValue(&curBalanceInfo.balanceRub));
  pcrfConfigObj->addInner(
      "balanceInfo", makeJsonRepeatObject(std::move(balanceInfoObj), [this]() {
        pcrfConfig.balanceInfo[curBalanceInfo.imsi] = curBalanceInfo;
      }));

  rootPtr->addInner("pcrfConfig", std::move(pcrfConfigObj));

  return root;
}

std::string Config::getBsFilePath() const { return bsFilePath; }
std::string Config::getEpcFilePath() const { return epcFilePath; }

std::vector<MmeConfig> Config::getMmeConfigs() const { return mmeConfigs; }

SmscConfig Config::getSmscConfig() const { return smscConfig; }
PcrfConfig Config::getPcrfConfig() const { return pcrfConfig; }
} // namespace server
