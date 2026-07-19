#pragma once

#include "common/json/json_parsable/json_parsable.h"

namespace server {
class EpcConfig : public common::JsonParsable {
private:
  unsigned int ttlSec = 0;
  std::string hlrSqliteFilePath = "";
  std::string eirSqliteFilePath = "";

  virtual std::unique_ptr<common::BaseJsonInfo> getJsonRootInfo() override;

public:
  unsigned int getTtlSec() const;
  std::string getHlrSqliteFilePath() const;
  std::string getEirSqliteFilePath() const;
};
} // namespace server
