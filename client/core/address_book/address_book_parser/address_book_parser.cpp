#include "address_book_parser.h"

namespace common {
AddressBookParser::AddressBookParser(const common::msisdn_t &curMsisdn_)
    : curMsisdn(curMsisdn_) {}

void AddressBookParser::initFields() {
  addParsedVector<common::msisdn_t>(
      "",
      [this](const std::vector<common::msisdn_t> &subscribers) {
        for (int i = 0; i < subscribers.size(); ++i) {
          char speedDialNum = (i <= 9) ? static_cast<char>(i) : '0';
          records.push_back({speedDialNum, subscribers[i]});
        }
      },
      nlohmann::json::value_t::string);
}

std::unique_ptr<AddressBookParser>
AddressBookParser::create(const common::msisdn_t &curMsisdn) {
  auto parser =
      std::unique_ptr<AddressBookParser>(new AddressBookParser(curMsisdn));
  parser->initFields();
  return parser;
}

std::expected<std::vector<AddressBookRecord>, std::string>
AddressBookParser::parseJson(const nlohmann::json &json) {
  // очистка конфига
  records.clear();

  auto error = this->parseFields(json);
  if (error) {
    return std::unexpected(*error);
  }

  return records;
}
} // namespace common
