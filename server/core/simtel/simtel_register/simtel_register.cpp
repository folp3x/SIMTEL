#include "simtel_register.h"

#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/subscriber_status/subscriber_status.h"

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
  try {
    auto records = hlrStorage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::msisdn) == msisdn));

    if (records.empty()) {
      return std::unexpected("HLR record not found for MSISDN: " + msisdn);
    }

    mmeId = records[0].mmeId;
    return records[0].imsi;
  } catch (const std::exception &e) {
    return std::unexpected("HLR error: " + std::string(e.what()));
  }
}

std::expected<common::msisdn_t, std::string>
SimtelRegister::getMsisdnByImsi(const common::imsi_t &imsi) {
  try {
    auto records = hlrStorage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));

    if (records.empty()) {
      return std::unexpected("HLR record not found for IMSI: " + imsi);
    }

    return records[0].msisdn;
  } catch (const std::exception &e) {
    return std::unexpected("HLR error: " + std::string(e.what()));
  }
}

std::expected<unsigned int, std::string>
SimtelRegister::getMmeIdByImsi(const common::imsi_t &imsi) {
  try {
    auto records = hlrStorage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));

    if (records.empty()) {
      return std::unexpected("HLR record not found for IMSI: " + imsi);
    }

    return records[0].mmeId;
  } catch (const std::exception &e) {
    return std::unexpected("HLR error: " + std::string(e.what()));
  }
}

void SimtelRegister::insertHlrData() {
  hlrStorage.insert(HlrRecord{0, "200000000000000", "89990000001"});
  hlrStorage.insert(HlrRecord{0, "400000000000000", "89990000002"});
  hlrStorage.insert(HlrRecord{0, "600000000000000", "89990000003"});
  hlrStorage.insert(HlrRecord{0, "800000000000000", "89990000004"});
}

bool SimtelRegister::hasHlrData() { return hlrStorage.count<HlrRecord>() != 0; }

void SimtelRegister::insertEirData() {
  eirStorage.insert(EirRecord{0, "100000000000000",
                              subscriberStatusToStr(SubscriberStatus::ACTIVE)});
  eirStorage.insert(EirRecord{0, "300000000000000",
                              subscriberStatusToStr(SubscriberStatus::ACTIVE)});
  eirStorage.insert(EirRecord{0, "500000000000000",
                              subscriberStatusToStr(SubscriberStatus::BANNED)});
  eirStorage.insert(EirRecord{0, "700000000000000",
                              subscriberStatusToStr(SubscriberStatus::ACTIVE)});
}

bool SimtelRegister::hasEirData() { return eirStorage.count<EirRecord>() != 0; }

std::expected<HlrRecord, std::string>
SimtelRegister::handleAuthInfoRequest(const common::imsi_t &imsi,
                                      const common::imei_t &imei) {
  try {
    auto eirRecords = eirStorage.get_all<EirRecord>(
        sqlite_orm::where(sqlite_orm::c(&EirRecord::imei) == imei));
    if (eirRecords.empty()) {
      return std::unexpected("EIR record not found for IMEI: " + imei);
    }

    EirRecord eirRecord = eirRecords[0];
    if (eirRecord.status == subscriberStatusToStr(SubscriberStatus::BANNED)) {
      return std::unexpected("UE is banned by IMEI");
    }

    auto hlrRecords = hlrStorage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));
    if (hlrRecords.empty()) {
      return std::unexpected("HLR record not found for IMSI: " + imsi);
    }

    return hlrRecords[0];
  } catch (const std::exception &e) {
    return std::unexpected("HLR error: " + std::string(e.what()));
  }
}

std::expected<std::optional<unsigned int>, std::string>
SimtelRegister::handleUpdateLocationRequest(const common::imsi_t &imsi,
                                            unsigned int mmeId) {
  try {
    auto records = hlrStorage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));

    if (records.empty()) {
      return std::unexpected("HLR record not found for IMSI: " + imsi);
    }

    HlrRecord record = records[0];
    std::optional<unsigned int> prevMmeId = record.mmeId;
    record.mmeId = mmeId;
    hlrStorage.update(record);

    auto updated = hlrStorage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));

    if (updated.empty()) {
      return std::unexpected("HLR error updating record for IMSI: " + imsi);
    }

    MessageHolder::instance().addMsg(
        createLogMsg("updated mmeId: " + updated[0].toStr()));

    return prevMmeId;
  } catch (const std::exception &e) {
    return std::unexpected("HLR error: " + std::string(e.what()));
  }
}

std::expected<HlrRecord, std::string>
SimtelRegister::handleRoutingInfoSmSender(const common::imsi_t &imsi) {
  try {
    auto records = hlrStorage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));

    if (records.empty()) {
      return std::unexpected("HLR record not found for IMSI: " + imsi);
    }

    HlrRecord record = records[0];

    MessageHolder::instance().addMsg(
        createLogMsg("found by imsi: " + record.toStr()));

    return record;
  } catch (const std::exception &e) {
    return std::unexpected("HLR error: " + std::string(e.what()));
  }
}

std::expected<HlrRecord, std::string>
SimtelRegister::handleRoutingInfoSmReceiver(const common::msisdn_t &msisdn) {
  try {
    auto records = hlrStorage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::msisdn) == msisdn));

    if (records.empty()) {
      return std::unexpected("HLR record not found for MSISDN: " + msisdn);
    }

    HlrRecord record = records[0];

    MessageHolder::instance().addMsg(
        createLogMsg("found by msisdn: " + record.toStr()));

    return record;
  } catch (const std::exception &e) {
    return std::unexpected("HLR error: " + std::string(e.what()));
  }
}
} // namespace server
