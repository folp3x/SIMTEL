#include "address_book.h"

#include "client/constants.h"
#include "common/validator/validator.h"

namespace client {
std::unique_ptr<common::BaseJsonInfo> AddressBook::getJsonRootInfo() {
  return makeJsonVector<common::msisdn_t>(&subscribers,
                                          common::Validator::isCorrectMsisdn);
}

std::map<char, common::msisdn_t> AddressBook::getRecords() const {
  if (records.empty() && !subscribers.empty()) {
    for (int i = 0; i < subscribers.size(); ++i) {
      if (i >= constants::MaxAddressBookSize) {
        break;
      }

      char speedDialNum =
          (i <= 9) ? static_cast<char>(i + '0') : constants::EmptySpeedDialNum;
      records[speedDialNum] = subscribers[i];
    }
  }

  return records;
}
} // namespace client
