#include "client/app/menu/command_parser/command_parser.h"

#include <gtest/gtest.h>

#include "client/app/menu/menu_item/menu_item_move/menu_item_move.h"
#include "common/app/menu/menu_item/menu_item_invalid/menu_item_invalid.h"

class CommandParserTest : public ::testing::Test {
protected:
  client::CommandParser parser{};
  std::string extraMsg = "";
};

TEST_F(CommandParserTest, Parse_MoveNoArg) {
  auto cmd = parser.parseCommand("move", extraMsg);
  auto invalidCmd = dynamic_cast<common::MenuItemInvalid *>(cmd.get());

  EXPECT_FALSE(invalidCmd == nullptr);
}

TEST_F(CommandParserTest, Parse_MoveRedundantArg) {
  auto cmd = parser.parseCommand("move 1 2 3 4", extraMsg);
  auto moveCmd = dynamic_cast<client::MenuItemMove<float> *>(cmd.get());

  client::MenuItemMove<float> correctCmd{{1, 2, 3}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
  EXPECT_FALSE(extraMsg.empty());
}

TEST_F(CommandParserTest, Parse_MoveInvalidArg) {
  auto cmd = parser.parseCommand("move str", extraMsg);
  auto invalidCmd = dynamic_cast<common::MenuItemInvalid *>(cmd.get());

  EXPECT_FALSE(invalidCmd == nullptr);
}

TEST_F(CommandParserTest, Parse_MoveOneFloatArg) {

  auto cmd = parser.parseCommand("move 1.1", extraMsg);
  auto moveCmd = dynamic_cast<client::MenuItemMove<float> *>(cmd.get());

  client::MenuItemMove<float> correctCmd{{1.1}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
}

TEST_F(CommandParserTest, Parse_MoveOneIntegerArg) {

  auto cmd = parser.parseCommand("move 1", extraMsg);
  auto moveCmd = dynamic_cast<client::MenuItemMove<float> *>(cmd.get());

  client::MenuItemMove<float> correctCmd{{1}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
}

TEST_F(CommandParserTest, Parse_MoveTwoArgs) {

  auto cmd = parser.parseCommand("move 1.1 2", extraMsg);
  auto moveCmd = dynamic_cast<client::MenuItemMove<float> *>(cmd.get());

  client::MenuItemMove<float> correctCmd{{1.1, 2}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
}

TEST_F(CommandParserTest, Parse_MoveThreeArgs) {

  auto cmd = parser.parseCommand("move 1.1 2 3", extraMsg);
  auto moveCmd = dynamic_cast<client::MenuItemMove<float> *>(cmd.get());

  client::MenuItemMove<float> correctCmd{{1.1, 2, 3}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
}

TEST_F(CommandParserTest, Parse_UnknownCommand) {
  std::string tempMsg = "";
  auto cmd = parser.parseCommand("123", tempMsg);
  auto unknownCmd = dynamic_cast<common::MenuItemInvalid *>(cmd.get());

  EXPECT_FALSE(unknownCmd == nullptr);
}

TEST_F(CommandParserTest, Parse_EmptyCommand) {
  auto cmd = parser.parseCommand("", extraMsg);
  auto unknownCmd = dynamic_cast<common::MenuItemInvalid *>(cmd.get());

  EXPECT_FALSE(unknownCmd == nullptr);
}
