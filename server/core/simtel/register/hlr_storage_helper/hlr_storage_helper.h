#pragma once

#include <sqlite_orm/sqlite_orm.h>

#include "server/core/simtel/register/hlr_record/hlr_record.h"

namespace server {
struct HlrStorageHelper {
  static auto create(const std::string &filePath) {
    return sqlite_orm::make_storage(
        filePath, sqlite_orm::make_table(
                      "hlr",
                      sqlite_orm::make_column("id", &HlrRecord::id,
                                              sqlite_orm::primary_key()),
                      sqlite_orm::make_column("imsi", &HlrRecord::imsi,
                                              sqlite_orm::unique()),
                      sqlite_orm::make_column("msisdn", &HlrRecord::msisdn,
                                              sqlite_orm::unique()),
                      sqlite_orm::make_column("mmeId", &HlrRecord::mmeId)));
  }
};
} // namespace server
