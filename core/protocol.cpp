#include "protocol.h"

#include <algorithm>

struct ProtocolInfo {
  Protocol value;
  std::string name = "";
};

static constexpr ProtocolInfo PROTOCOLS[] = {{Protocol::BINARY, "binary"},
                                             {Protocol::JSON, "json"}};

// ищет информацию о протоколе по названию
auto findProtocolByName(const std::string &name) {
  return std::find_if(std::begin(PROTOCOLS), std::end(PROTOCOLS),
                      [&name](const auto &info) { return info.name == name; });
}

// ищет информацию о протоколе по значению
auto findProtocolByValue(Protocol value) {
  return std::find_if(
      std::begin(PROTOCOLS), std::end(PROTOCOLS),
      [&value](const auto &info) { return info.value == value; });
}

// возвращает протокол в виде строки
std::string protocolToStr(Protocol p) {
  auto it = findProtocolByValue(p);
  return (it == std::end(PROTOCOLS)) ? "unknown" : it->name;
}

// преобразует строку в значение протокола
std::optional<Protocol> protocolFromStr(const std::string &str) {
  auto it = findProtocolByName(str);
  if (it != std::end(PROTOCOLS)) {
    return it->value;
  }
  return std::nullopt;
}

// проверяет содержит ли строка корректное значение протокола
bool isCorrectProtocolStr(const std::string &str) {
  auto it = findProtocolByName(str);
  return it != std::end(PROTOCOLS);
}
