#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>

enum class Protocol { JSON, BINARY };

std::string protocolToStr(Protocol protocol);
Protocol protocolFromStr(const std::string &str);
bool isCorrectProtocolStr(const std::string &str);

#endif // PROTOCOL_H
