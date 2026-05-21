#include "command_parser.h"

namespace server {
CommandParser::ArgsParsersMap CommandParser::getArgsParsers() const {
  return argsParsers;
}
} // namespace server
