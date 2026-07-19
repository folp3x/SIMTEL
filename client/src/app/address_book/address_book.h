#pragma once

#include <map>

#include "common/json/json_parsable/json_parsable.h"
#include "common/types.h"

namespace client {
class AddressBook : public common::JsonParsable {
public:
  using records_t = std::map<char, common::msisdn_t>;

private:
  std::vector<common::msisdn_t> subscribers{};
  mutable records_t records{};

  virtual std::unique_ptr<common::BaseJsonInfo> getJsonRootInfo() override;

public:
  records_t getRecords() const;
};
} // namespace client
