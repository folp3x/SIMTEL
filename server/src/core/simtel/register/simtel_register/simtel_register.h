#pragma once

#include <expected>
#include <memory>
#include <string>

#include "core/simtel/register/eir_storage_helper/eir_storage_helper.h"
#include "core/simtel/register/hlr_storage_helper/hlr_storage_helper.h"

namespace server {
class SimtelRegister {
private:
  using hlr_storage_t = decltype(HlrStorageHelper::create(""));
  using eir_storage_t = decltype(EirStorageHelper::create(""));

  hlr_storage_t hlrStorage;
  eir_storage_t eirStorage;

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

  template <typename Condition>
  std::expected<std::vector<HlrRecord>, std::string>
  findInHlr(const Condition &condition);

  template <typename Condition>
  std::expected<std::vector<EirRecord>, std::string>
  findInEir(const Condition &condition);

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

#include "simtel_register_impl.h"
