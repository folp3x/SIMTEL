#pragma once

#include <optional>
#include <zpp_bits.h>

#include "common/types.h"

namespace common {
class BaseBinaryInfo {
protected:
  using big_endian_t = zpp::bits::endian::big;

  using in_stream_t = zpp::bits::in<const common::binary_t, big_endian_t>;

  using out_stream_t = zpp::bits::out<common::binary_t, big_endian_t>;

public:
  virtual ~BaseBinaryInfo() = default;

  virtual std::optional<std::string> parse(in_stream_t &stream) = 0;

  virtual std::optional<std::string> write(out_stream_t &stream) = 0;
};
} // namespace common
