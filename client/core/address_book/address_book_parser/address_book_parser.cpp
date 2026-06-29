#include "address_book_parser.h"

#include "client/constants.h"

namespace client {
void AddressBookParser::initFields() {
  addParsedVector<common::msisdn_t>(
      "", [this](const std::vector<common::msisdn_t> &subscribers) {
        for (int i = 0; i < subscribers.size(); ++i) {
          if (i >= constants::MAX_ADDRESS_BOOK_SIZE) {
            break;
          }

          char speedDialNum = (i <= 9) ? static_cast<char>(i + '0')
                                       : constants::EMPTY_SPEED_DIAL_NUM;
          records[speedDialNum] = subscribers[i];
        }
      });
}

std::unique_ptr<AddressBookParser> AddressBookParser::create() {
  auto parser = std::unique_ptr<AddressBookParser>(new AddressBookParser());
  parser->initFields();
  return parser;
}

std::expected<std::map<char, common::msisdn_t>, std::string>
AddressBookParser::parseJson(const nlohmann::json &json) {
  records.clear();

  auto error = this->parseFields(json);
  if (error) {
    return std::unexpected(*error);
  }

  return records;
}
} // namespace client
