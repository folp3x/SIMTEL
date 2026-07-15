#include "command_parser.h"

#include "client/app/menu/menu_item/menu_item_active/menu_item_active.h"
#include "client/app/menu/menu_item/menu_item_dialog/menu_item_dialog.h"
#include "client/app/menu/menu_item/menu_item_protocol/menu_item_protocol.h"
#include "client/app/menu/menu_item/menu_item_sms/menu_item_sms.h"
#include "client/app/menu/menu_item/menu_item_ussd_code/menu_item_ussd_code.h"

#include "client/validator/validator.h"
#include "common/network/protocol/protocol.h"
#include "common/validator/validator.h"

namespace client {
CommandParser::arg_parser_map_t CommandParser::getArgsParsers() const {
  return argsParsers;
}

std::unique_ptr<common::MenuItem>
CommandParser::parseExitArgs(const std::string &initialStr,
                             const std::vector<std::string> &args,
                             std::string &extraMsg) {
  if (args.size() > MenuItemExit::getArgsCount()) {
    extraMsg = "Extra arguments ignored";
  }

  return std::make_unique<MenuItemExit>();
}

std::unique_ptr<common::MenuItem>
CommandParser::parseActiveArgs(const std::string &initialStr,
                               const std::vector<std::string> &args,
                               std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>("Missing argument");
  }

  std::string isActiveStr = args[0];

  auto isActiveValue = common::utils::parseBool(isActiveStr);
  if (isActiveValue) {
    if (args.size() > MenuItemActive::getArgsCount()) {
      extraMsg = "Extra arguments ignored";
    }
    return std::make_unique<MenuItemActive>(*isActiveValue);
  }

  return std::make_unique<common::MenuItemInvalid>("Invalid argument");
}

std::unique_ptr<common::MenuItem>
CommandParser::parseUssdCodeArgs(const std::string &initialStr,
                                 const std::vector<std::string> &args,
                                 std::string &extraMsg) {
  std::string command = common::utils::firstWord(initialStr);

  size_t minLength = UssdPrefixLength + 1 + UssdPostfixLength;
  if (command.size() < minLength) {
    return std::make_unique<common::MenuItemInvalid>("Missing argument");
  }

  size_t lastCodeChInd = command.size() - 1 - UssdPostfixLength;
  std::string codeStr = command.substr(UssdPrefixLength, lastCodeChInd);

  auto code = common::utils::fromString<unsigned int>(codeStr);
  if (code) {
    return std::make_unique<MenuItemUssdCode>(*code);
  }

  return std::make_unique<common::MenuItemInvalid>("Invalid argument");
}

std::unique_ptr<common::MenuItem>
CommandParser::parseCommand(const std::string &str,
                            std::string &extraMsg) const {
  auto cmd = common::CommandParser::parseCommand(str, extraMsg);
  if (dynamic_cast<common::MenuItemInvalid *>(cmd.get())) {
    std::vector<std::string> tokens = common::utils::split(str);

    if (tokens.empty()) {
      return cmd;
    }

    std::string command = common::utils::lowercased(tokens[0]);
    // удаление названия команды
    tokens.erase(tokens.begin());

    size_t lastChInd = command.size() - 1;
    if (command[0] == UssdPrefix && command[lastChInd] == UssdPostfix) {
      return parseUssdCodeArgs(command, tokens, extraMsg);
    }
  }

  return cmd;
}

std::unique_ptr<common::MenuItem>
CommandParser::parseProtocolArgs(const std::string &initialStr,
                                 const std::vector<std::string> &args,
                                 std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>("Missing argument");
  }

  std::string value = args[0];

  auto foundName = common::protocolNameFromAlias(value);
  std::string name = value;
  if (foundName) {
    name = std::move(*foundName);
  }

  if (common::isCorrectProtocolStr(name)) {
    if (args.size() > MenuItemProtocol::getArgsCount()) {
      extraMsg = "Extra arguments ignored";
    }
    return std::make_unique<MenuItemProtocol>(name);
  }
  return std::make_unique<common::MenuItemInvalid>("Invalid argument");
}

std::unique_ptr<common::MenuItem>
CommandParser::parseSmsArgs(const std::string &initialStr,
                            const std::vector<std::string> &args,
                            std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>("Missing argument");
  }

  auto cmd = std::make_unique<MenuItemSMS>();
  std::string firstArg = args[0];
  if (firstArg[0] == SpeedDialNumPrefix) {
    std::string speedDialNumStr = firstArg.substr(1);
    std::string error = Validator::isCorrectSpeedDialNumStr(speedDialNumStr);
    if (!error.empty()) {
      return std::make_unique<common::MenuItemInvalid>(error);
    }
    char speedDialNum = speedDialNumStr[0];

    cmd->setSpeedDialNum(speedDialNum);
  } else {
    common::msisdn_t msisdn = args[0];
    std::string error = common::Validator::isCorrectMsisdn(msisdn);
    if (!error.empty()) {
      return std::make_unique<common::MenuItemInvalid>(error);
    }

    cmd->setMsisdn(msisdn);
  }

  cmd->setContent(common::utils::ignoreWords(initialStr, 2));

  return cmd;
}

std::unique_ptr<common::MenuItem>
CommandParser::parseDialogArgs(const std::string &initialStr,
                               const std::vector<std::string> &args,
                               std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>("Missing argument");
  }

  auto cmd = std::make_unique<MenuItemDialog>();
  std::string firstArg = args[0];
  if (firstArg[0] == SpeedDialNumPrefix) {
    std::string speedDialNumStr = firstArg.substr(1);
    std::string error = Validator::isCorrectSpeedDialNumStr(speedDialNumStr);
    if (!error.empty()) {
      return std::make_unique<common::MenuItemInvalid>(error);
    }
    char speedDialNum = speedDialNumStr[0];

    cmd->setSpeedDialNum(speedDialNum);
  } else {
    common::msisdn_t msisdn = args[0];
    std::string error = common::Validator::isCorrectMsisdn(msisdn);
    if (!error.empty()) {
      return std::make_unique<common::MenuItemInvalid>(error);
    }

    cmd->setMsisdn(msisdn);
  }

  if (args.size() > MenuItemDialog::getArgsCount()) {
    extraMsg = "Extra arguments ignored";
  }
  return cmd;
}
} // namespace client
