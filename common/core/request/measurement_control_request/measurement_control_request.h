#pragma once

#include "common/core/request/request/request.h"

namespace common {
class MeasurementControlRequest : public Request {
  imei_t imei = "";
  unsigned int signal = 0;
  unsigned int bsId = 0;

public:
  MeasurementControlRequest() = default;
  MeasurementControlRequest(const imei_t &imei_, unsigned int signal_,
                            unsigned int bsId_);

  virtual std::string toStr() const override;

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const common::binary_t &binary);

  imei_t getImei() const;
  unsigned int getSignal() const;
  unsigned int getBsId() const;
};
} // namespace common
