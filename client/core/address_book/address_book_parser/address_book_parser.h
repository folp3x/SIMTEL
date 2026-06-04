#pragma once

#include "common/json/json_parser/json_parser.h"

#include <vector>

#include "client/core/address_book/address_book_record/address_book_record.h"

namespace common {
// класс для парсинга адресной книги из JSON
class AddressBookParser : public JsonParser<std::vector<AddressBookRecord>> {
private:
  common::msisdn_t curMsisdn = "";
  std::vector<AddressBookRecord> records{};

  explicit AddressBookParser(const common::msisdn_t &curMsisdn_);

  virtual void initFields() override;

public:
  static std::unique_ptr<AddressBookParser>
  create(const common::msisdn_t &curMsisdn);

  virtual std::expected<std::vector<AddressBookRecord>, std::string>
  parseJson(const nlohmann::json &json) override;
};
} // namespace common
