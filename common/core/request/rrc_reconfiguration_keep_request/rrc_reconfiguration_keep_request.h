#pragma once

#include "common/core/request/request/request.h"

namespace common {
class RrcReconfigurationKeepRequest : public Request {
private:
  imei_t imei = "";
  unsigned int bsId = 0;

public:
  RrcReconfigurationKeepRequest() = default;
  RrcReconfigurationKeepRequest(const imei_t &imei_, unsigned int bsId_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const common::binary_t &binary);

  imei_t getImei() const;
  unsigned int getBsId() const;
};
} // namespace common
