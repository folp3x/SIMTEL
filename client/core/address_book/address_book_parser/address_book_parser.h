#pragma once

#include "common/json/json_parser/json_parser.h"

#include <map>

#include "common/types.h"

namespace client {
// класс для парсинга адресной книги из JSON
class AddressBookParser
    : public common::JsonParser<std::map<char, common::msisdn_t>> {
private:
  common::msisdn_t curMsisdn = "";
  std::map<char, common::msisdn_t> records{};

  explicit AddressBookParser(const common::msisdn_t &curMsisdn_);

  virtual void initFields() override;

  virtual std::expected<std::map<char, common::msisdn_t>, std::string>
  parseJson(const nlohmann::json &json) override;

public:
  static std::unique_ptr<AddressBookParser>
  create(const common::msisdn_t &curMsisdn);
};
} // namespace client
