#include "protocol.h"

#include <algorithm>
#include <stdexcept>

struct ProtocolInfo {
  Protocol value;
  std::string name;
};

static const ProtocolInfo PROTOCOLS[] = {{Protocol::BINARY, "binary"},
                                         {Protocol::JSON, "json"}};

// возвращает протокол в виде строки
std::string protocolToStr(Protocol p) {
  // поиск значения в списке протоколов
  auto it = std::find_if(std::begin(PROTOCOLS), std::end(PROTOCOLS),
                         [&p](const auto &info) { return info.value == p; });
  return (it == std::end(PROTOCOLS)) ? "unknown" : it->name;
}

// преобразует строку в значение протокола
Protocol protocolFromStr(const std::string &str) {
  // поиск значения в списке протоколов
  auto it = std::find_if(std::begin(PROTOCOLS), std::end(PROTOCOLS),
                         [&str](const auto &info) { return info.name == str; });
  if (it != std::end(PROTOCOLS)) {
    return it->value;
  }
  throw std::invalid_argument("Unknown protocol");
}

// проверяет содержит ли строка корректное значение протокола
bool isCorrectProtocolStr(const std::string &str) {
  // поиск значения в списке протоколов
  auto it = std::find_if(std::begin(PROTOCOLS), std::end(PROTOCOLS),
                         [&str](const auto &info) { return info.name == str; });
  return it != std::end(PROTOCOLS);
}
