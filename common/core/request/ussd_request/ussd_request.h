#pragma once

#include "common/core/request/request/request.h"

namespace common {
class UssdRequest : public Request {
private:
  uint8_t code = 0;

public:
  UssdRequest() = default;
  explicit UssdRequest(uint8_t code_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const binary_t &binary);

  uint8_t getCode() const;
};
} // namespace common
