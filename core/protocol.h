#pragma once

#include <string>

enum class Protocol { JSON, BINARY };

std::string protocolToStr(Protocol protocol);
Protocol protocolFromStr(const std::string &str);
bool isCorrectProtocolStr(const std::string &str);
