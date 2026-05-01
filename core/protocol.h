#pragma once

#include <string>
#include <optional>

enum class Protocol { JSON, BINARY };

static auto findProtocolByName(const std::string &name);
static auto findProtocolByValue(Protocol value);

std::string protocolToStr(Protocol protocol);
std::optional<Protocol> protocolFromStr(const std::string &str);
bool isCorrectProtocolStr(const std::string &str);
