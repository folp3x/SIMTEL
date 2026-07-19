#pragma once

#include <string>

namespace common {
enum class NetworkErrorType : uint8_t {
  SendTimeout,
  ConnectionClosed,
  Other,
  EmptyMessage,
  LargeMessage,
  ReceiveTimeout,
  BadFileDescriptor,
  IncompleteHeader,
  NoMsgSize,
  IncompleteContent
};

std::string networkErrorTypeToStr(NetworkErrorType type);
} // namespace common
