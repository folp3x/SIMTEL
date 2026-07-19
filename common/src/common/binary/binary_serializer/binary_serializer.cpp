#include "binary_serializer.h"

#include <zpp_bits.h>

#include "common/utils/network/network.h"
#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace common {
binary_t BinarySerializer::strToBinaryUnsized(const std::string &binary) {
  binary_t result;
  result.reserve(binary.size());
  for (auto byte : binary) {
    result.push_back(static_cast<std::byte>(byte));
  }
  return result;
}

std::string BinarySerializer::strFromBinaryUnsized(const binary_t &binary) {
  std::string result;
  result.reserve(binary.size());
  for (std::byte byte : binary) {
    result.push_back(static_cast<char>(byte));
  }
  return result;
}

std::expected<RequestType, std::string>
BinarySerializer::parseRequestType(const binary_t &bytes) {
  auto header = socketMessageHeaderFromBinary(bytes);
  if (!header) {
    return std::unexpected("Failed to deserialize header");
  }

  return static_cast<RequestType>(header->reqType);
}

std::expected<SocketMessage, std::string>
BinarySerializer::socketMessageFromBinary(const binary_t &binary) {
  SocketMessage message;

  auto parsedHeader = socketMessageHeaderFromBinary(binary);
  if (!parsedHeader) {
    return std::unexpected("Failed to deserialize header");
  }
  message.header = *parsedHeader;

  if (binary.size() <
      constants::SocketMessageHeaderBytes + message.header.msgSize) {
    return std::unexpected("Failed to deserialize content");
  }

  message.content.assign(binary.begin() + constants::SocketMessageHeaderBytes,
                         binary.end());

  return message;
}

std::expected<binary_t, std::string>
BinarySerializer::socketMessageToBinary(const SocketMessage &msg) {
  auto parsedHeader = socketMessageHeaderToBinary(msg.header);
  if (!parsedHeader) {
    return std::unexpected("Failed to serialize header");
  }

  return utils::mergeBinary(*parsedHeader, msg.content);
}

std::optional<SocketMessageHeader>
BinarySerializer::socketMessageHeaderFromBinary(const binary_t &binary) {
  SocketMessageHeader header;

  zpp::bits::in in{binary, zpp::bits::endian::big{}};
  auto result = in(header.msgSize, header.protocol, header.reqType);
  if (zpp::bits::failure(result)) {
    return std::nullopt;
  }

  return header;
}

std::optional<binary_t> BinarySerializer::socketMessageHeaderToBinary(
    const SocketMessageHeader &header) {
  common::binary_t data{};
  zpp::bits::out out{data, zpp::bits::endian::big{}};

  auto result = out(header.msgSize, header.protocol, header.reqType);
  if (zpp::bits::failure(result)) {
    return std::nullopt;
  }

  return data;
}
} // namespace common
