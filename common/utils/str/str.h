#pragma once

#include <expected>
#include <optional>

#include <stdexcept>

#include <string>
#include <vector>

#include "common/constants.h"

namespace common::utils {
std::string lowercased(std::string_view str);
std::string uppercased(std::string_view str);

std::optional<bool> parseBool(std::string_view str);

bool allDigits(std::string_view str);

std::vector<std::string> split(const std::string &str);

std::string imsiToStr(uint64_t imsi);
std::string imeiToStr(uint64_t imei);

std::optional<uint64_t> identifierFromStr(const std::string &str);

template <typename T>
  requires std::is_floating_point_v<T>
std::string
toStr(T num,
      std::optional<unsigned int> precision_ = constants::RealNumPrecision,
      bool fixed = false);

// итератор, указывающий на числовой элемент контейнера
// решение найдено здесь:
// https://softwareengineering.stackexchange.com/questions/291803/template-function-passing-iterators
template <std::input_iterator Iterator,
          typename U = std::iter_value_t<Iterator>>
  requires std::is_arithmetic_v<U>
std::string
toStr(Iterator begin, Iterator end,
      std::optional<unsigned int> precision = constants::RealNumPrecision,
      char leftBorder = '[', char rightBorder = ']');

template <typename T>
  requires std::is_arithmetic_v<T>
std::expected<T, std::string> fromString(const std::string &str);

std::string firstWord(const std::string &str);
std::string ignoreWords(const std::string &str, size_t count);

std::string addLeadingZeroes(const std::string &str, size_t length);

template <typename T>
std::optional<T> toOptional(const std::expected<T, std::string> &result);

std::string quoted(const std::string &str);
} // namespace common::utils

#include "str_impl.h"
