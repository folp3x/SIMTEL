#pragma once

#include "common/core/request/request/request.h"

namespace common {
class SmDeliveryReportResponse : public Request {
private:
  imsi_t mTimsi = "";
  unsigned int smsId = 0;

protected:
  static constexpr size_t BinaryBytesCount = constants::ImsiBinaryBytes + sizeof(smsId);

public:
  SmDeliveryReportResponse() = default;
  SmDeliveryReportResponse(const common::imsi_t &mTimsi_, unsigned int smsId_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const binary_t &binary);

  imsi_t getMTimsi() const;
  unsigned int getSmsId() const;
};
} // namespace common
