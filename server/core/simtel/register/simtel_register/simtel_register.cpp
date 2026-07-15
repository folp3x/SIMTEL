#include "simtel_register.h"

#include "server/app/menu/message_holder/message_holder.h"
#include "server/core/simtel/register/subscriber_status/subscriber_status.h"

namespace server {
std::string SimtelRegister::createLogMsg(const std::string &content) const {
  return "HLR/EIR: " + content;
}

void SimtelRegister::logRecords() {
  try {
    auto hlrRecords = hlrStorage.get_all<HlrRecord>();

    MessageHolder::instance().addMsg("HLR records:");
    for (const auto &record : hlrRecords) {
      MessageHolder::instance().addMsg(record.toStr());
    }

    auto eirRecords = eirStorage.get_all<EirRecord>();

    MessageHolder::instance().addMsg("EIR records:");
    for (const auto &record : eirRecords) {
      MessageHolder::instance().addMsg(record.toStr());
    }
  } catch (const std::exception &e) {
    MessageHolder::instance().addErrorMsg(
        createLogMsg("failed to read records: " + std::string(e.what())));
  }
}

SimtelRegister::SimtelRegister(const std::string &hlrSqliteFilePath,
                               const std::string &eirSqliteFilePath)
    : hlrStorage(HlrStorageHelper::create(hlrSqliteFilePath)),
      eirStorage(EirStorageHelper::create(eirSqliteFilePath)) {
  hlrStorage.sync_schema();
  eirStorage.sync_schema();
}

std::expected<common::imsi_t, std::string>
SimtelRegister::getImsiByMsisdn(const common::imsi_t &msisdn,
                                unsigned int &mmeId) {
  auto records =
      findInHlr(sqlite_orm::where(sqlite_orm::c(&HlrRecord::msisdn) == msisdn));
  if (!records) {
    return std::unexpected(records.error());
  }

  if (records->empty()) {
    return std::unexpected("HLR record not found for MSISDN: " + msisdn);
  }

  HlrRecord record = (*records)[0];
  mmeId = record.mmeId;

  return record.imsi;
}

std::expected<common::msisdn_t, std::string>
SimtelRegister::getMsisdnByImsi(const common::imsi_t &imsi) {
  auto records =
      findInHlr(sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));
  if (!records) {
    return std::unexpected(records.error());
  }

  if (records->empty()) {
    return std::unexpected("HLR record not found for IMSI: " + imsi);
  }

  return (*records)[0].msisdn;
}

std::expected<unsigned int, std::string>
SimtelRegister::getMmeIdByImsi(const common::imsi_t &imsi) {
  auto records =
      findInHlr(sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));
  if (!records) {
    return std::unexpected(records.error());
  }

  if (records->empty()) {
    return std::unexpected("HLR record not found for IMSI: " + imsi);
  }

  return (*records)[0].mmeId;
}

void SimtelRegister::insertHlrData() {
  hlrStorage.insert(HlrRecord{0, "200000000000000", "89990000001"});
  hlrStorage.insert(HlrRecord{0, "400000000000000", "89990000002"});
  hlrStorage.insert(HlrRecord{0, "600000000000000", "89990000003"});
  hlrStorage.insert(HlrRecord{0, "800000000000000", "89990000004"});
}

bool SimtelRegister::hasHlrData() { return hlrStorage.count<HlrRecord>() != 0; }

void SimtelRegister::insertEirData() {
  std::string activeStatusStr = subscriberStatusToStr(SubscriberStatus::Active);
  std::string bannedStatusStr = subscriberStatusToStr(SubscriberStatus::Banned);

  eirStorage.insert(EirRecord{0, "100000000000000", activeStatusStr});
  eirStorage.insert(EirRecord{0, "300000000000000", activeStatusStr});
  eirStorage.insert(EirRecord{0, "500000000000000", bannedStatusStr});
  eirStorage.insert(EirRecord{0, "700000000000000", activeStatusStr});
}

bool SimtelRegister::hasEirData() { return eirStorage.count<EirRecord>() != 0; }

std::expected<HlrRecord, std::string>
SimtelRegister::handleAuthInfoRequest(const common::imsi_t &imsi,
                                      const common::imei_t &imei) {
  auto eirRecords =
      findInEir(sqlite_orm::where(sqlite_orm::c(&EirRecord::imei) == imei));
  if (!eirRecords) {
    return std::unexpected(eirRecords.error());
  }

  if (eirRecords->empty()) {
    return std::unexpected("EIR record not found for IMEI: " + imei);
  }

  EirRecord eirRecord = (*eirRecords)[0];
  if (eirRecord.status == subscriberStatusToStr(SubscriberStatus::Banned)) {
    return std::unexpected("UE is banned by IMEI");
  }

  auto hlrRecords = findInHlr(sqlite_orm::where(
      sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi)));
  if (!hlrRecords) {
    return std::unexpected(hlrRecords.error());
  }

  if (hlrRecords->empty()) {
    return std::unexpected("HLR record not found for IMSI: " + imsi);
  }

  return (*hlrRecords)[0];
}

std::expected<std::optional<unsigned int>, std::string>
SimtelRegister::handleUpdateLocationRequest(const common::imsi_t &imsi,
                                            unsigned int mmeId) {
  auto records = findInHlr(sqlite_orm::where(
      sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi)));
  if (!records) {
    return std::unexpected(records.error());
  }

  if (records->empty()) {
    return std::unexpected("HLR record not found for IMSI: " + imsi);
  }

  HlrRecord record = (*records)[0];
  unsigned int prevMmeId = record.mmeId;
  record.mmeId = mmeId;

  try {
    hlrStorage.update(record);
  } catch (const std::exception &e) {
    return std::unexpected("HLR error: " + std::string(e.what()));
  }

  auto updated =
      findInHlr(sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));
  if (!updated) {
    return std::unexpected(updated.error());
  }

  if (updated->empty()) {
    return std::unexpected("HLR error updating record for IMSI: " + imsi);
  }

  MessageHolder::instance().addMsg(
      createLogMsg("updated mmeId: " + (*updated)[0].toStr()));

  return prevMmeId;
}

std::expected<HlrRecord, std::string>
SimtelRegister::handleRoutingInfoSmSender(const common::imsi_t &imsi) {
  auto records =
      findInHlr(sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));
  if (!records) {
    return std::unexpected(records.error());
  }

  if (records->empty()) {
    return std::unexpected("HLR record not found for IMSI: " + imsi);
  }

  HlrRecord record = (*records)[0];

  MessageHolder::instance().addMsg(
      createLogMsg("found by imsi: " + record.toStr()));

  return record;
}

std::expected<HlrRecord, std::string>
SimtelRegister::handleRoutingInfoSmReceiver(const common::msisdn_t &msisdn) {
  auto records =
      findInHlr(sqlite_orm::where(sqlite_orm::c(&HlrRecord::msisdn) == msisdn));
  if (!records) {
    return std::unexpected(records.error());
  }

  if (records->empty()) {
    return std::unexpected("HLR record not found for MSISDN: " + msisdn);
  }

  HlrRecord record = (*records)[0];

  MessageHolder::instance().addMsg(
      createLogMsg("found by msisdn: " + record.toStr()));

  return record;
}
} // namespace server
