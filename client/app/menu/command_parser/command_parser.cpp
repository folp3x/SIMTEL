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
CommandParser::ArgsParsersMap CommandParser::getArgsParsers() const {
  return argsParsers;
}

std::unique_ptr<common::MenuItem>
CommandParser::parseExitArgs(const std::vector<std::string> &args,
                             std::string &extraMsg) {
  if (args.size() > MenuItemExit::getArgsCount()) {
    extraMsg = "Extra arguments ignored";
  }

  return std::make_unique<MenuItemExit>();
}

std::unique_ptr<common::MenuItem>
CommandParser::parseActiveArgs(const std::vector<std::string> &args,
                               std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>("Missing argument");
  }

  std::string isActiveStr = args[0];

  auto isActiveValue = common::parseBool(isActiveStr);
  if (isActiveValue) {
    if (args.size() > MenuItemActive::getArgsCount()) {
      extraMsg = "Extra arguments ignored";
    }
    return std::make_unique<MenuItemActive>(*isActiveValue);
  }

  return std::make_unique<common::MenuItemInvalid>("Invalid argument");
}

std::unique_ptr<common::MenuItem>
CommandParser::parseUssdCodeArgs(const std::vector<std::string> &args,
                                 std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>("Missing command");
  }

  std::string command = args[0];

  size_t minLength =
      constants::USSD_PREFIX_LENGTH + 1 + constants::USSD_POSTFIX_LENGTH;
  if (command.size() < minLength) {
    return std::make_unique<common::MenuItemInvalid>("Missing argument");
  }

  size_t lastCodeChInd = command.size() - 1 - constants::USSD_POSTFIX_LENGTH;
  std::string codeStr =
      command.substr(constants::USSD_PREFIX_LENGTH, lastCodeChInd);

  auto code = common::fromString<unsigned int>(codeStr);
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
    std::vector<std::string> tokens = common::split(common::lowercased(str));

    if (tokens.empty()) {
      return cmd;
    }

    std::string command = tokens[0];

    size_t lastChInd = command.size() - 1;
    if (command[0] == constants::USSD_PREFIX &&
        command[lastChInd] == constants::USSD_POSTFIX) {
      return parseUssdCodeArgs(tokens, extraMsg);
    }
  }

  return cmd;
}

std::unique_ptr<common::MenuItem>
CommandParser::parseProtocolArgs(const std::vector<std::string> &args,
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
CommandParser::parseSmsArgs(const std::vector<std::string> &args,
                            std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>("Missing argument");
  }

  auto cmd = std::make_unique<MenuItemSMS>();
  std::string firstArg = args[0];
  if (firstArg[0] == constants::SPEED_DIAL_NUM_PREFIX) {
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

  if (args.size() > 1) {
    cmd->setContent(args[1]);
  }

  if (args.size() > MenuItemSMS::getMaxArgsCount()) {
    extraMsg = "Extra arguments ignored";
  }
  return cmd;
}

std::unique_ptr<common::MenuItem>
CommandParser::parseDialogArgs(const std::vector<std::string> &args,
                               std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>("Missing argument");
  }

  auto cmd = std::make_unique<MenuItemDialog>();
  std::string firstArg = args[0];
  if (firstArg[0] == constants::SPEED_DIAL_NUM_PREFIX) {
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
