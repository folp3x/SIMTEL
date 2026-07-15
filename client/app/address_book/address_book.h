#pragma once

#include <map>

#include "common/json/json_parsable/json_parsable.h"
#include "common/types.h"

namespace client {
class AddressBook : public common::JsonParsable {
private:
  std::vector<common::msisdn_t> subscribers{};
  mutable std::map<char, common::msisdn_t> records{};

  virtual std::unique_ptr<common::BaseJsonInfo> getJsonRootInfo() override;

public:
  std::map<char, common::msisdn_t> getRecords() const;
};
} // namespace client
