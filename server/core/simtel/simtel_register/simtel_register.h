#pragma once

#include <expected>
#include <memory>
#include <string>

#include "server/core/simtel/eir_storage_helper/eir_storage_helper.h"
#include "server/core/simtel/hlr_storage_helper/hlr_storage_helper.h"

namespace server {
class SimtelRegister {
private:
  using HlrStorageType = decltype(HlrStorageHelper::create(""));
  using EirStorageType = decltype(EirStorageHelper::create(""));

  HlrStorageType hlrStorage;
  EirStorageType eirStorage;

  std::string createLogMsg(const std::string &content) const;

public:
  SimtelRegister(const std::string &hlrSqliteFilePath,
                 const std::string &eirSqliteFilePath);

  std::expected<common::imsi_t, std::string>
  getImsiByMsisdn(const common::imsi_t &msisdn, unsigned int &mmeId);

  std::expected<common::msisdn_t, std::string>
  getMsisdnByImsi(const common::imsi_t &imsi);

  std::expected<unsigned int, std::string>
  getMmeIdByImsi(const common::imsi_t &imsi);

  void insertHlrData();
  bool hasHlrData();

  void insertEirData();
  bool hasEirData();

  std::expected<HlrRecord, std::string>
  handleAuthInfoRequest(const common::imsi_t &imsi, const common::imei_t &imei);

  std::expected<std::optional<unsigned int>, std::string>
  handleUpdateLocationRequest(const common::imsi_t &imsi, unsigned int mmeId);

  std::expected<HlrRecord, std::string>
  handleRoutingInfoSmReceiver(const common::msisdn_t &msisdn);

  std::expected<HlrRecord, std::string>
  handleRoutingInfoSmSender(const common::imsi_t &imsi);

  void logRecords();
};
} // namespace server
