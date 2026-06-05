#include "address_book_parser.h"

#include "client/constants.h"

namespace client {
AddressBookParser::AddressBookParser(const common::msisdn_t &curMsisdn_)
    : curMsisdn(curMsisdn_) {}

void AddressBookParser::initFields() {
  addParsedVector<common::msisdn_t>(
      "",
      [this](const std::vector<common::msisdn_t> &subscribers) {
        for (int i = 0; i < subscribers.size(); ++i) {
          if (i >= constants::MAX_ADDRESS_BOOK_SIZE) {
            break;
          }

          char speedDialNum = (i <= 9) ? static_cast<char>(i + '0')
                                       : constants::EMPTY_SPEED_DIAL_NUM;
          records[speedDialNum] = subscribers[i];
        }
      },
      nlohmann::json::value_t::string,
      [this](const common::msisdn_t &msisdn) { return msisdn != curMsisdn; });
}

std::unique_ptr<AddressBookParser>
AddressBookParser::create(const common::msisdn_t &curMsisdn) {
  auto parser =
      std::unique_ptr<AddressBookParser>(new AddressBookParser(curMsisdn));
  parser->initFields();
  return parser;
}

std::expected<std::map<char, common::msisdn_t>, std::string>
AddressBookParser::parseJson(const nlohmann::json &json) {
  // очистка конфига
  records.clear();

  auto error = this->parseFields(json);
  if (error) {
    return std::unexpected(*error);
  }

  return records;
}
} // namespace client
