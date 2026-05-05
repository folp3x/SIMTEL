#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <string>
#include <variant>

struct InvalidCommand {
  std::string error = "";
};

struct UnknownCommand {};

struct ExitCommand {};

struct ActiveCommand {
  bool isActive = false;
};

struct MoveCommand {
  static const int COORDS_COUNT = 3;
  std::vector<double> coords = {};
};

struct ProtocolCommand {
  std::string value = "";
};

// аналог union для удобного определения типа команды
using Command = std::variant<InvalidCommand, UnknownCommand, ExitCommand,
                             ActiveCommand, MoveCommand, ProtocolCommand>;

#endif // COMMAND_H
