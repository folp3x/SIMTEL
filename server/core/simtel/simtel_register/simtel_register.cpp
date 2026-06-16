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
                    sqlite_orm::make_column("mmeId", &HlrRecord::mmeId)));
}

std::string SimtelRegister::createLogMsg(const std::string &content) const {
  return "HLR: " + content;
}

SimtelRegister::SimtelRegister(const std::string &hlrSqliteFilePath)
    : storage(createStorage(hlrSqliteFilePath)) {
  storage.sync_schema();
}

void SimtelRegister::insertData() {
  storage.insert(
      HlrRecord{0, "100000000000000", "200000000000000", "89990000001",
                subscriberStatusToStr(SubscriberStatus::ACTIVE), std::nullopt});
  storage.insert(
      HlrRecord{0, "300000000000000", "400000000000000", "89990000002",
                subscriberStatusToStr(SubscriberStatus::ACTIVE), std::nullopt});
  storage.insert(
      HlrRecord{0, "500000000000000", "600000000000000", "89990000003",
                subscriberStatusToStr(SubscriberStatus::BANNED), std::nullopt});
}

bool SimtelRegister::hasData() { return storage.count<HlrRecord>() != 0; }

std::expected<HlrRecord, std::string>
SimtelRegister::handleAuthInfoRequest(const common::imsi_t &imsi,
                                      const common::imei_t &imei) {
  try {
    auto records = storage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));

    if (records.empty()) {
      return std::unexpected("Record not found for IMSI: " + imsi);
    }

    HlrRecord record = records[0];
    if (record.status == subscriberStatusToStr(SubscriberStatus::BANNED)) {
      return std::unexpected("UE is banned");
    }

    if (record.imei != imei) {
      return std::unexpected("Expected IMEI -" + record.imei + ", got - " +
                             imei);
    }

    MessageHolder::instance().addMsg(
        createLogMsg("found record: " + record.toStr()));

    return record;
  } catch (const std::exception &e) {
    return std::unexpected("HLR DB error: " + std::string(e.what()));
  }
}

std::optional<std::string>
SimtelRegister::handleUpdateLocationRequest(const common::imsi_t &imsi,
                                            unsigned int mmeId) {
  try {
    auto records = storage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));

    if (records.empty()) {
      return "Record not found for IMSI: " + imsi;
    }

    storage.update_all(
        sqlite_orm::set(sqlite_orm::c(&HlrRecord::mmeId) = mmeId),
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::imsi) == imsi));

    MessageHolder::instance().addMsg(
        createLogMsg("updated record: [imsi=" + imsi +
                     ", mmeId=" + std::to_string(mmeId) + "]"));

    return std::nullopt;
  } catch (const std::exception &e) {
    return "HLR DB error: " + std::string(e.what());
  }
}

std::expected<HlrRecord, std::string>
SimtelRegister::handleRoutingInfoSM(const common::msisdn_t &msisdn) {
  try {
    auto records = storage.get_all<HlrRecord>(
        sqlite_orm::where(sqlite_orm::c(&HlrRecord::msisdn) == msisdn));

    if (records.empty()) {
      return std::unexpected("Record not found for MSISDN: " + msisdn);
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
