#pragma once

#include "common/json/json_parsable/json_parsable.h"

namespace common {
class Config : public common::JsonParsable {
private:
  int port = 0;

protected:
  virtual std::unique_ptr<common::BaseJsonInfo> getJsonRootInfo() override;

public:
  virtual ~Config() = default;

  int getPort() const;
  void setPort(int port_);
};
} // namespace common
