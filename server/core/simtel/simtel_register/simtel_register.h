#pragma once

#include <expected>
#include <memory>
#include <sqlite_orm/sqlite_orm.h>
#include <string>

#include "server/core/simtel/hlr_record/hlr_record.h"

namespace server {
class SimtelRegister {
private:
  using StorageType = decltype(sqlite_orm::make_storage(
      "", sqlite_orm::make_table(
              "hlr",
              sqlite_orm::make_column("id", &HlrRecord::id,
                                      sqlite_orm::primary_key()),
              sqlite_orm::make_column("imsi", &HlrRecord::imsi,
                                      sqlite_orm::unique()),
              sqlite_orm::make_column("imei", &HlrRecord::imei,
                                      sqlite_orm::unique()),
              sqlite_orm::make_column("msisdn", &HlrRecord::msisdn,
                                      sqlite_orm::unique()),
              sqlite_orm::make_column("status", &HlrRecord::status),
              sqlite_orm::make_column("mmeId", &HlrRecord::mmeId))));

  StorageType storage;

  auto createStorage(const std::string &filePath);

  std::string createLogMsg(const std::string &content) const;

public:
  explicit SimtelRegister(const std::string &hlrSqliteFilePath);

  void insertData();
  bool hasData();

  std::optional<std::string> handleAuthInfoRequest(const common::imsi_t &imsi,
                                                   const common::imei_t &imei);

  std::optional<std::string>
  handleUpdateLocationRequest(const common::imsi_t &imsi, unsigned int mmeId);

  std::expected<HlrRecord, std::string>
  handleRoutingInfoSM(const common::msisdn_t &msisdn);
};
} // namespace server
