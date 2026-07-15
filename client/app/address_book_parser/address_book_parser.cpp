#include "address_book_parser.h"

#include "client/constants.h"

namespace client {
void AddressBookParser::initFields() {
  addInfo(makeParsedVector<common::msisdn_t>(
      "", [this](const std::vector<common::msisdn_t> &subscribers) {
        for (int i = 0; i < subscribers.size(); ++i) {
          if (i >= constants::MaxAddressBookSize) {
            break;
          }

          char speedDialNum = (i <= 9) ? static_cast<char>(i + '0')
                                       : constants::EmptySpeedDialNum;
          records[speedDialNum] = subscribers[i];
        }
      }));
}

std::unique_ptr<AddressBookParser> AddressBookParser::create() {
  auto parser = std::unique_ptr<AddressBookParser>(new AddressBookParser());
  parser->initFields();
  return parser;
}

std::expected<std::map<char, common::msisdn_t>, std::string>
AddressBookParser::parseJson(const nlohmann::json &json) {
  records.clear();

  auto error = parseFields(json);
  if (error) {
    return std::unexpected(*error);
  }

  return records;
}
} // namespace client
