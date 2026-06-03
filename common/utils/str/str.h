#pragma once

#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace common {
std::string lowercased(std::string_view str);
std::string uppercased(std::string_view str);
std::optional<bool> parseBool(std::string_view str);
bool allDigits(std::string_view str);
std::string ltrimmed(std::string_view str);
std::vector<std::string> split(const std::string &str);

template <typename T>
  requires std::is_floating_point_v<T>
std::string toStr(T num, std::optional<unsigned int> precision_ = 4);

// итератор указывающий на числовой элемент контейнера
// решение найдено здесь:
// https://softwareengineering.stackexchange.com/questions/291803/template-function-passing-iterators
template <class Iterator,
          class U = typename std::iterator_traits<Iterator>::value_type>
  requires std::is_arithmetic_v<U>
std::string toStr(Iterator begin, Iterator end,
                  std::optional<unsigned int> precision = 4,
                  char leftBorder = '[', char rightBorder = ']');

template <typename T>
  requires std::is_arithmetic_v<T>
std::expected<T, std::string> fromString(const std::string &str);
} // namespace common

#include "str_impl.h"
