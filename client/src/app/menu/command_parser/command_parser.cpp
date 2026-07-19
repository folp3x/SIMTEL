#include "command_parser.h"

#include "app/menu/menu_item/menu_item_active/menu_item_active.h"
#include "app/menu/menu_item/menu_item_dialog/menu_item_dialog.h"
#include "app/menu/menu_item/menu_item_protocol/menu_item_protocol.h"
#include "app/menu/menu_item/menu_item_sms/menu_item_sms.h"
#include "app/menu/menu_item/menu_item_ussd_code/menu_item_ussd_code.h"

#include "common/network/protocol/protocol.h"
#include "common/validator/validator.h"
#include "validator/validator.h"

namespace client {
CommandParser::arg_parser_map_t CommandParser::getArgsParsers() const {
  return argsParsers;
}

std::expected<char, std::string>
CommandParser::parseSpeedDialNum(const std::string &arg) {
  std::string speedDialNumStr = arg.substr(1);
  std::string error = Validator::isCorrectSpeedDialNumStr(speedDialNumStr);
  if (!error.empty()) {
    return std::unexpected(error);
  }
  return speedDialNumStr[0];
}

std::expected<common::msisdn_t, std::string>
CommandParser::parseMsisdn(const std::string &arg) {
  std::string error = common::Validator::isCorrectMsisdn(arg);
  if (!error.empty()) {
    return std::unexpected(error);
  }

  return arg;
}

std::unique_ptr<common::MenuItem>
CommandParser::parseExitArgs(const std::string &initialStr,
                             const std::vector<std::string> &args,
                             std::string &extraMsg) {
  if (args.size() > MenuItemExit::getArgsCount()) {
    extraMsg = ExtraArgsMsg;
  }

  return std::make_unique<MenuItemExit>();
}

std::unique_ptr<common::MenuItem>
CommandParser::parseActiveArgs(const std::string &initialStr,
                               const std::vector<std::string> &args,
                               std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>(MissingArgMsg);
  }

  std::string isActiveStr = args[0];

  auto isActiveValue = common::utils::parseBool(isActiveStr);
  if (isActiveValue) {
    if (args.size() > MenuItemActive::getArgsCount()) {
      extraMsg = ExtraArgsMsg;
    }
    return std::make_unique<MenuItemActive>(*isActiveValue);
  }

  return std::make_unique<common::MenuItemInvalid>(InvalidArgMsg);
}

std::unique_ptr<common::MenuItem>
CommandParser::parseUssdCodeArgs(const std::string &initialStr,
                                 const std::vector<std::string> &args,
                                 std::string &extraMsg) {
  std::string command = common::utils::firstWord(initialStr);

  size_t minLength = UssdPrefixLength + 1 + UssdPostfixLength;
  if (command.size() < minLength) {
    return std::make_unique<common::MenuItemInvalid>(MissingArgMsg);
  }

  size_t lastCodeChInd = command.size() - 1 - UssdPostfixLength;
  std::string codeStr = command.substr(UssdPrefixLength, lastCodeChInd);

  auto code = common::utils::fromString<unsigned int>(codeStr);
  if (code) {
    return std::make_unique<MenuItemUssdCode>(*code);
  }

  return std::make_unique<common::MenuItemInvalid>(InvalidArgMsg);
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
    return std::make_unique<common::MenuItemInvalid>(MissingArgMsg);
  }

  std::string value = args[0];

  auto foundName = common::protocolNameFromAlias(value);
  std::string name = value;
  if (foundName) {
    name = std::move(*foundName);
  }

  if (common::isCorrectProtocolStr(name)) {
    if (args.size() > MenuItemProtocol::getArgsCount()) {
      extraMsg = ExtraArgsMsg;
    }
    return std::make_unique<MenuItemProtocol>(name);
  }
  return std::make_unique<common::MenuItemInvalid>(InvalidArgMsg);
}

std::unique_ptr<common::MenuItem>
CommandParser::parseSmsArgs(const std::string &initialStr,
                            const std::vector<std::string> &args,
                            std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>(MissingArgMsg);
  }

  auto cmd = std::make_unique<MenuItemSMS>();
  std::string firstArg = args[0];
  if (firstArg[0] == SpeedDialNumPrefix) {
    auto num = parseSpeedDialNum(firstArg);
    if (!num) {
      return std::make_unique<common::MenuItemInvalid>(num.error());
    }

    cmd->setSpeedDialNum(*num);
  } else {
    auto msisdn = parseMsisdn(firstArg);
    if (!msisdn) {
      return std::make_unique<common::MenuItemInvalid>(msisdn.error());
    }

    cmd->setMsisdn(*msisdn);
  }

  cmd->setContent(common::utils::ignoreWords(initialStr, 2));

  return cmd;
}

std::unique_ptr<common::MenuItem>
CommandParser::parseDialogArgs(const std::string &initialStr,
                               const std::vector<std::string> &args,
                               std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>(MissingArgMsg);
  }

  auto cmd = std::make_unique<MenuItemDialog>();
  std::string firstArg = args[0];
  if (firstArg[0] == SpeedDialNumPrefix) {
    auto num = parseSpeedDialNum(firstArg);
    if (!num) {
      return std::make_unique<common::MenuItemInvalid>(num.error());
    }

    cmd->setSpeedDialNum(*num);
  } else {
    auto msisdn = parseMsisdn(firstArg);
    if (!msisdn) {
      return std::make_unique<common::MenuItemInvalid>(msisdn.error());
    }

    cmd->setMsisdn(*msisdn);
  }

  if (args.size() > MenuItemDialog::getArgsCount()) {
    extraMsg = ExtraArgsMsg;
  }
  return cmd;
}
} // namespace client
