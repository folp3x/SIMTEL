#include "address_book.h"

#include "common/utils/num/num.h"
#include "common/validator/validator.h"
#include "constants.h"

namespace client {
std::unique_ptr<common::BaseJsonInfo> AddressBook::getJsonRootInfo() {
  return makeJsonVector<common::msisdn_t>(&subscribers,
                                          common::Validator::isCorrectMsisdn);
}

AddressBook::records_t AddressBook::getRecords() const {
  if (records.empty() && !subscribers.empty()) {
    for (size_t i = 0; i < subscribers.size(); ++i) {
      if (i >= constants::MaxAddressBookSize) {
        break;
      }

      char speedDialNum = (i <= 9) ? common::utils::numToDigit(i)
                                   : constants::EmptySpeedDialNum;
      records[speedDialNum] = subscribers[i];
    }
  }

  return records;
}
} // namespace client
