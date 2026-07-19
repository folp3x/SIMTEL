#pragma once

namespace common::utils {
template <typename T>
  requires std::is_floating_point_v<T>
std::string toStr(T num, std::optional<unsigned int> precision_, bool fixed) {
  std::stringstream stream;
  stream.imbue(std::locale());

  if (precision_) {
    if (fixed) {
      stream << std::fixed << std::setprecision(*precision_);
    } else {
      stream << std::setprecision(*precision_);
    }
  }

  stream << num;
  return stream.str();
}

template <std::input_iterator Iterator, typename U>
  requires std::is_arithmetic_v<U>
std::string toStr(Iterator begin, Iterator end,
                  std::optional<unsigned int> precision, char leftBorder,
                  char rightBorder) {
  if (begin == end) {
    return std::to_string(leftBorder) + std::to_string(rightBorder);
  }

  std::string str(1, leftBorder);
  auto last = std::prev(end);

  for (auto it = begin; it != end; ++it) {
    if constexpr (std::is_floating_point_v<U>) {
      str += toStr<U>(*it, precision);
    } else {
      str += std::to_string(*it);
    }

    if (it != last) {
      str += ", ";
    }
  }
  str += rightBorder;

  return str;
}

template <typename T>
  requires std::is_arithmetic_v<T>
std::expected<T, std::string> fromString(const std::string &str) {
  const std::string ConvertError = "Narrowing conversion";

  try {
    if constexpr (std::is_unsigned_v<T>) {
      return std::stoull(str);
    } else if constexpr (std::is_integral_v<T>) {
      return std::stoll(str);
    } else {
      return stold(str);
    }
  } catch (std::invalid_argument &e) {
    return std::unexpected("Not a valid number");
  } catch (std::out_of_range &e) {
    return std::unexpected("Value out of range");
  } catch (std::exception &e) {
    return std::unexpected("Failed to parse number");
  }
}

template <typename T>
std::optional<T> toOptional(const std::expected<T, std::string> &result) {
  return result ? std::optional{*result} : std::nullopt;
}
} // namespace common::utils
