#pragma once

#include "common/core/request/request/request.h"

namespace common {
class UssdMsisdnResponse : public Request {
private:
  msisdn_t msisdn = "";

public:
  UssdMsisdnResponse() = default;
  UssdMsisdnResponse(const msisdn_t &msisdn_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const binary_t &binary);

  msisdn_t getMsisdn() const;
};
} // namespace common
