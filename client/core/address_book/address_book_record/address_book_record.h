#pragma once

#include <cstdint>

#include "common/types.h"

struct AddressBookRecord {
  char speedDialNum = '0';
  common::msisdn_t msisdn = "";
};
