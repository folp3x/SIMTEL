#pragma once

#include "common/core/response/sm_delivery_report_response/sm_delivery_report_response.h"

namespace common {
class SmDeliveryErrorResponse : public SmDeliveryReportResponse {
private:
  std::string description = "";

public:
  SmDeliveryErrorResponse() = default;
  SmDeliveryErrorResponse(const imsi_t &mTimsi, unsigned int smsId,
                          const std::string &description_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const binary_t &binary);

  std::string getDescription() const;
};
} // namespace common
