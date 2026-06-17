#include "simtel_register.h"

#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/subscriber_status/subscriber_status.h"

namespace server {
auto SimtelRegister::createStorage(const std::string &filePath) {
  return sqlite_orm::make_storage(
      filePath, sqlite_orm::make_table(
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
                    sqlite_orm::make_column("mmeId", &HlrRecord::mmeId),
                    sqlite_orm::make_column("mTimsi", &HlrRecord::mTimsi)));
}

std::string SimtelRegister::createLogMsg(const std::string &content) const {
  return "HLR: " + content;
}

SimtelRegister::SimtelRegister(const std::string &hlrSqliteFilePath)
    : storage(createStorage(hlrSqliteFilePath)) {
  storage.sync_schema();
}

std::expected<common::imsi_t, std::string>
SimtelRegister::getImsiByMTimsi(const common::imsi_t &mTimsi,
                                unsigned int mmeId) {
  try {
    auto records = storage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::mTimsi) == mTimsi &&
                          sqlite_orm::c(&HlrRecord::mmeId) == mmeId));

    if (records.empty()) {
      return std::unexpected("HLR record not found for m-timsi: " + mTimsi);
    }

    return records[0].imsi;
  } catch (const std::exception &e) {
    return std::unexpected("HLR DB error: " + std::string(e.what()));
  }
}

void SimtelRegister::insertData() {
  storage.insert(HlrRecord{0, "100000000000000", "200000000000000",
                           "89990000001",
                           subscriberStatusToStr(SubscriberStatus::ACTIVE),
                           std::nullopt, std::nullopt});
  storage.insert(HlrRecord{0, "300000000000000", "400000000000000",
                           "89990000002",
                           subscriberStatusToStr(SubscriberStatus::ACTIVE),
                           std::nullopt, std::nullopt});
  storage.insert(HlrRecord{0, "500000000000000", "600000000000000",
                           "89990000003",
                           subscriberStatusToStr(SubscriberStatus::BANNED),
                           std::nullopt, std::nullopt});

  try {
    auto records = storage.get_all<HlrRecord>();

    MessageHolder::instance().addMsg(createLogMsg("added records"));

    for (const auto &record : records) {
      MessageHolder::instance().addMsg(createLogMsg(record.toStr()));
    }
  } catch (const std::exception &e) {
    MessageHolder::instance().addErrorMsg(
        createLogMsg("error reading records: " + std::string(e.what())));
  }
}

bool SimtelRegister::hasData() { return storage.count<HlrRecord>() != 0; }

std::expected<HlrRecord, std::string>
SimtelRegister::handleAuthInfoRequest(const common::imsi_t &imsi,
                                      const common::imei_t &imei,
                                      const common::imsi_t &mTimsi) {
  try {
    auto records = storage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));

    if (records.empty()) {
      return std::unexpected("HLR record not found for IMSI: " + imsi);
    }

    MessageHolder::instance().addMsg(
        createLogMsg("found record: " + records[0].toStr()));

    HlrRecord record = records[0];
    if (record.status == subscriberStatusToStr(SubscriberStatus::BANNED)) {
      return std::unexpected("UE is banned");
    }

    if (record.imei != imei) {
      return std::unexpected("Expected IMEI -" + record.imei + ", got - " +
                             imei);
    }

    record.mTimsi = mTimsi;
    storage.update(record);

    MessageHolder::instance().addMsg(
        createLogMsg("updated record: " + record.toStr()));

    return record;
  } catch (const std::exception &e) {
    return std::unexpected("HLR DB error: " + std::string(e.what()));
  }
}

std::expected<std::optional<unsigned int>, std::string>
SimtelRegister::handleUpdateLocationRequest(const common::imsi_t &imsi,
                                            unsigned int mmeId) {
  try {
    auto records = storage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));

    if (records.empty()) {
      return std::unexpected("HLR record not found for IMSI: " + imsi);
    }

    HlrRecord record = records[0];
    std::optional<unsigned int> prevMmeId = record.mmeId;
    record.mmeId = mmeId;
    storage.update(record);

    auto updated = storage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));

    if (updated.empty()) {
      return std::unexpected("HLR error updating record for IMSI: " + imsi);
    }

    MessageHolder::instance().addMsg(
        createLogMsg("updated record: " + updated[0].toStr()));

    return prevMmeId;
  } catch (const std::exception &e) {
    return std::unexpected("HLR DB error: " + std::string(e.what()));
  }
}

std::expected<HlrRecord, std::string>
SimtelRegister::handleRoutingInfoSmSender(const common::imsi_t &imsi) {
  try {
    auto records = storage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));

    if (records.empty()) {
      return std::unexpected("HLR record not found for IMSI: " + imsi);
    }

    HlrRecord record = records[0];

    MessageHolder::instance().addMsg(
        createLogMsg("found record: " + record.toStr()));

    return record;
  } catch (const std::exception &e) {
    return std::unexpected("HLR DB error: " + std::string(e.what()));
  }
}

std::expected<HlrRecord, std::string>
SimtelRegister::handleRoutingInfoSmReceiver(const common::msisdn_t &msisdn) {
  try {
    auto records = storage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::msisdn) == msisdn));

    if (records.empty()) {
      return std::unexpected("HLR record not found for MSISDN: " + msisdn);
    }

    HlrRecord record = records[0];

    MessageHolder::instance().addMsg(
        createLogMsg("found record: " + record.toStr()));

    return record;
  } catch (const std::exception &e) {
    return std::unexpected("HLR DB error: " + std::string(e.what()));
  }
}
} // namespace server
