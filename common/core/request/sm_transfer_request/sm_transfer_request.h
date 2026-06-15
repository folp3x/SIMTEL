#pragma once

#include "common/core/request/request/request.h"

namespace common {
class SmTransferRequest : public Request {
private:
  static constexpr size_t TEXT_CHARS_SHOWED = 10;

  imsi_t mTimsi = "";
  unsigned int smsId = 0;
  msisdn_t msisdn = "";
  std::string text = "";

protected:
  std::string getValuesStr() const;

public:
  SmTransferRequest() = default;
  SmTransferRequest(const imsi_t &mTimsi_, unsigned int smsId_,
                    const common::msisdn_t &msisdn_, const std::string &text_);

  virtual std::string toStr() const override;

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const binary_t &binary);

  imsi_t getMTimsi() const;
  unsigned int getSmsId() const;
  msisdn_t getMsisdn() const;
  std::string getText() const;
};
} // namespace common
