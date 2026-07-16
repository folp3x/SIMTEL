#include "epc_config.h"

#include "common/validator/validator.h"

namespace server {
std::unique_ptr<common::BaseJsonInfo> EpcConfig::getJsonRootInfo() {
  auto root = makeJsonObject();

  root->addInner("ttl_sec",
                 makeJsonValue<unsigned int>(&ttlSec, [](unsigned int ttl) {
                   return common::Validator::isPositiveNumber(ttl, "TTL");
                 }));

  auto hlrAccessParamsObj = makeJsonObject();
  hlrAccessParamsObj->addInner("sqliteFilePath",
                               makeJsonValue(&hlrSqliteFilePath));

  root->addInner("hlrAccessParams", std::move(hlrAccessParamsObj));

  auto eirAccessParamsObj = makeJsonObject();
  eirAccessParamsObj->addInner("sqliteFilePath",
                               makeJsonValue(&eirSqliteFilePath));

  root->addInner("eirAccessParams", std::move(eirAccessParamsObj));

  return root;
}

unsigned int EpcConfig::getTtlSec() const { return ttlSec; }

std::string EpcConfig::getHlrSqliteFilePath() const {
  return hlrSqliteFilePath;
}

std::string EpcConfig::getEirSqliteFilePath() const {
  return eirSqliteFilePath;
}
} // namespace server
