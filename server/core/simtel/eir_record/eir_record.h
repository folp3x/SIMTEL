#pragma once

#include "common/types.h"

namespace server {
struct EirRecord {
  int id;
  common::imei_t imei;
  std::string status;

  std::string toStr() const {
    return "[id=" + std::to_string(id) + ", imei=" + imei +
           ", status=" + status + "]";
  }
};
} // namespace server
