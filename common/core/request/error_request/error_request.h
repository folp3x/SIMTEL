#pragma once

#include "common/core/request/request/request.h"

namespace common {
class ErrorRequest : public Request {
private:
  std::string description = "";

public:
  ErrorRequest() = default;
  ErrorRequest(const std::string &description_);

  virtual std::string toStr() const override;

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const common::binary_t &binary);

  std::string getDescription() const;
};
} // namespace common
