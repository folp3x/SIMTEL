#pragma once

#include "common/core/request/request/request.h"

namespace common {
class RrcReconfigurationHandoverResponse : public Request {
private:
  imsi_t mTimsi = "";
  unsigned int bsId = 0;

public:
  RrcReconfigurationHandoverResponse() = default;
  RrcReconfigurationHandoverResponse(const imsi_t &mTimsi_, unsigned int bsId_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

  imsi_t getMTimsi() const;
  unsigned int getBsId() const;
};
} // namespace common
