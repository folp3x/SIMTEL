#pragma once

#include "common/core/request/request/request.h"

namespace common {
class UssdCodeRequest : public Request {
private:
  imsi_t mTimsi = "";
  uint8_t code = 0;

public:
  UssdCodeRequest() = default;
  UssdCodeRequest(const imsi_t &mTimsi_, uint8_t code_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

  imsi_t getMTimsi() const;
  uint8_t getCode() const;
};
} // namespace common
