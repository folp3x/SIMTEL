#pragma once

#include <sqlite_orm/sqlite_orm.h>

#include "server/core/simtel/register/eir_record/eir_record.h"

namespace server {
struct EirStorageHelper {
  static auto create(const std::string &filePath) {
    return sqlite_orm::make_storage(
        filePath, sqlite_orm::make_table(
                      "eir",
                      sqlite_orm::make_column("id", &EirRecord::id,
                                              sqlite_orm::primary_key()),
                      sqlite_orm::make_column("imei", &EirRecord::imei,
                                              sqlite_orm::unique()),
                      sqlite_orm::make_column("status", &EirRecord::status)));
  }
};
} // namespace server
