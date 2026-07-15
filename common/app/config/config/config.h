#pragma once

#include "common/json/json_parsable/json_parsable.h"

namespace common {
class Config : public common::JsonParsable {
private:
  static constexpr int InvalidPort = -1;

  int port = InvalidPort;

protected:
  virtual std::unique_ptr<common::BaseJsonInfo> getJsonRootInfo() override;

public:
  virtual ~Config() = default;

  virtual bool isInitialized() const;

  int getPort() const;
  void setPort(int port_);
};
} // namespace common
