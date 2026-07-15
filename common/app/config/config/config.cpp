#include "config.h"

#include "common/validator/validator.h"

namespace common {
std::unique_ptr<common::BaseJsonInfo> Config::getJsonRootInfo() {
  auto root = makeJsonObject();
  root->addInner("port", makeJsonValue<int>(&port, Validator::isCorrectPort));
  return root;
}

bool Config::isInitialized() const { return port != InvalidPort; }

int Config::getPort() const { return port; }

void Config::setPort(int port_) { port = port_; }
} // namespace common
