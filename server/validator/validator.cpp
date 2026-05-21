#include "validator.h"

namespace server {
std::string Validator::isCorrectNodesPath(const std::string &filePath) {
  return common::Validator::jsonFilePathExists(filePath, "Nodes");
}
} // namespace server
