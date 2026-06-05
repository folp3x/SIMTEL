#include "command_parser.h"

#include "client/app/menu/menu_item/menu_item_active/menu_item_active.h"
#include "client/app/menu/menu_item/menu_item_dialog/menu_item_dialog.h"
#include "client/app/menu/menu_item/menu_item_protocol/menu_item_protocol.h"
#include "client/app/menu/menu_item/menu_item_sms/menu_item_sms.h"
#include "client/validator/validator.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"
#include "common/utils/str/str.h"
#include "common/validator/validator.h"

namespace client {
CommandParser::ArgsParsersMap CommandParser::getArgsParsers() const {
  return argsParsers;
}

std::unique_ptr<common::MenuItem>
CommandParser::parseActiveArgs(const std::vector<std::string> &args,
                               std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>("Missing argument");
  }

  std::string isActiveStr = args[0];

  auto parseResult = common::parseBool(isActiveStr);
  if (parseResult) {
    if (args.size() > MenuItemActive::getArgsCount()) {
      extraMsg = "Extra arguments ignored";
    }
    return std::make_unique<MenuItemActive>(*parseResult);
  }

  return std::make_unique<common::MenuItemInvalid>("Invalid argument");
}

std::unique_ptr<common::MenuItem>
CommandParser::parseProtocolArgs(const std::vector<std::string> &args,
                                 std::string &extraMsg) {
  if (args.empty()) {
    return std::make_unique<common::MenuItemInvalid>("Missing argument");
  }

  std::string value = args[0];

  auto nameFindResult = common::protocolNameFromAlias(value);
  std::string name;
  if (nameFindResult) {
    name = *nameFindResult;
  } else {
    name = value;
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
