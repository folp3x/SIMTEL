#pragma once

namespace common {
enum class NetworkErrorType {
  SendTimeout,
  ConnectionClosed,
  Other,
  EmptyMessage,
  EmptyMessageHeader,
  LargeMessage,
  ReceiveTimeout,
  ConnectionReset,
  BadFileDescriptor,
  IncompleteHeader,
  NoMsgSize,
  IncompleteContent
};
};
