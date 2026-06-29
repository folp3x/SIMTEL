#pragma once

#include "common/core/request/request/request.h"

namespace common {
class MeasurementReportRequest : public Request {
private:
  imei_t imei = "";
  imei_t imsi = "";
  unsigned int bsId = 0;

public:
  MeasurementReportRequest() = default;
  MeasurementReportRequest(const imei_t &imei_, const imsi_t &imsi_,
                           unsigned int bsId_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const binary_t &binary);

  imei_t getImei() const;
  imei_t getImsi() const;
  unsigned int getBsId() const;
};
} // namespace common
