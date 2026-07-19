#pragma once

#include "common/core/request/request/request.h"

namespace common {
class MeasurementReportRequest : public Request {
private:
  imei_t imei = "";
  imei_t imsi = "";
  unsigned int bsId = 0;

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

public:
  MeasurementReportRequest() = default;
  MeasurementReportRequest(const imei_t &imei_, const imsi_t &imsi_,
                           unsigned int bsId_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;

  imei_t getImei() const;
  imei_t getImsi() const;
  unsigned int getBsId() const;
};
} // namespace common
