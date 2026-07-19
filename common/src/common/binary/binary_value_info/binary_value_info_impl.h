#pragma once

namespace common {
template <typename T, typename S>
template <typename V>
std::expected<V, std::string>
BinaryValueInfo<T, S>::valueFromBinary(in_stream_t &stream) {
  V value;

  if (zpp::bits::failure(stream(value))) {
    return std::unexpected("Deserialization error at byte " +
                           std::to_string(stream.position()));
  }

  return value;
}

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

template <typename T, typename S>
template <typename V>
std::optional<std::string>
BinaryValueInfo<T, S>::valueToBinary(out_stream_t &stream, const V &value) {
  if (zpp::bits::failure(stream(value))) {
    return "Serialization error at byte " + std::to_string(stream.position());
  }

  return std::nullopt;
}

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

template <typename T, typename S>
BinaryValueInfo<T, S>::BinaryValueInfo(
    T *value_, const std::function<std::string(const T &)> &validateFunc_,
    const std::function<std::optional<S>(const T &)> &serializeFunc_,
    const std::function<std::optional<T>(const S &)> &deserializeFunc_)
    : value(value_), serializeFunc(serializeFunc_),
      deserializeFunc(deserializeFunc_), validateFunc(validateFunc_) {}

template <typename T, typename S>
std::optional<std::string> BinaryValueInfo<T, S>::parse(in_stream_t &stream) {
  T parsedValue;

  if constexpr (std::is_same_v<T, S>) {
    if (!deserializeFunc) {
      auto parsed = valueFromBinary<T>(stream);
      if (!parsed) {
        return parsed.error();
      }
      parsedValue = *parsed;
    } else {
      auto parsed = valueFromBinary<S>(stream);
      if (!parsed) {
        return parsed.error();
      }

      auto result = deserializeFunc(*parsed);
      if (!result) {
        return "Failed to deserialize value";
      }

      parsedValue = std::move(*result);
    }
  } else {
    if (!deserializeFunc) {
      throw std::invalid_argument(
          "Deserialization function is required for different types");
    }

    auto parsed = valueFromBinary<S>(stream);
    if (!parsed) {
      return parsed.error();
    }

    auto result = deserializeFunc(*parsed);
    if (!result) {
      return "Failed to deserialize value";
    }

    parsedValue = std::move(*result);
  }

  if (validateFunc) {
    std::string error = validateFunc(parsedValue);
    if (!error.empty()) {
      return error;
    }
  }

  *value = std::move(parsedValue);

  return std::nullopt;
}

template <typename T, typename S>
std::optional<std::string> BinaryValueInfo<T, S>::write(out_stream_t &stream) {
  if constexpr (std::is_same_v<T, S>) {
    if (!serializeFunc) {
      return valueToBinary(stream, *value);
    } else {
      auto serialized = serializeFunc(*value);
      if (!serialized) {
        return "Failed to serialize value";
      }

      return valueToBinary(stream, *serialized);
    }
  } else {
    if (!serializeFunc) {
      throw std::invalid_argument(
          "Serialization function is required for different types");
    }

    auto serialized = serializeFunc(*value);
    if (!serialized) {
      return "Failed to serialize value";
    }

    return valueToBinary(stream, *serialized);
  }
}
} // namespace common
