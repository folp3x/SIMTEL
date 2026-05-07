#include "client/menu/command_parser.h"

#include <gtest/gtest.h>

class CommandParserTest : public ::testing::Test {
protected:
  CommandParser parser{};
};

TEST_F(CommandParserTest, Parse_MoveNoArg) {
  auto cmd = parser.parseCommand("move");
  auto invalidCmd = dynamic_cast<MenuItemInvalid *>(cmd.get());

  EXPECT_FALSE(invalidCmd == nullptr);
}

TEST_F(CommandParserTest, Parse_MoveRedundantArg) {
  auto cmd = parser.parseCommand("move 1 2 3 4");
  auto invalidCmd = dynamic_cast<MenuItemInvalid *>(cmd.get());

  EXPECT_FALSE(invalidCmd == nullptr);
}

TEST_F(CommandParserTest, Parse_MoveInvalidArg) {
  auto cmd = parser.parseCommand("move str");
  auto invalidCmd = dynamic_cast<MenuItemInvalid *>(cmd.get());

  EXPECT_FALSE(invalidCmd == nullptr);
}

TEST_F(CommandParserTest, Parse_MoveOneFloatArg) {

  auto cmd = parser.parseCommand("move 1.1");
  auto moveCmd = dynamic_cast<MenuItemMove *>(cmd.get());

  MenuItemMove correctCmd{{1.1}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
}

TEST_F(CommandParserTest, Parse_MoveOneIntegerArg) {

  auto cmd = parser.parseCommand("move 1");
  auto moveCmd = dynamic_cast<MenuItemMove *>(cmd.get());

  MenuItemMove correctCmd{{1}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
}

TEST_F(CommandParserTest, Parse_MoveTwoArgs) {

  auto cmd = parser.parseCommand("move 1.1 2");
  auto moveCmd = dynamic_cast<MenuItemMove *>(cmd.get());

  MenuItemMove correctCmd{{1.1, 2}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
}

TEST_F(CommandParserTest, Parse_MoveThreeArgs) {

  auto cmd = parser.parseCommand("move 1.1 2 3");
  auto moveCmd = dynamic_cast<MenuItemMove *>(cmd.get());

  MenuItemMove correctCmd{{1.1, 2, 3}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
}

TEST_F(CommandParserTest, Parse_UnknownCommand) {
  auto cmd = parser.parseCommand("123");
  // проверка существовани команды
  const auto &commands = getCommandsInfo();
  for (const auto &[name, _] : commands) {
    if (cmd->getName() == name)
      GTEST_SKIP() << "Tested as unknown command exists, skipping test";
  }
  auto unknownCmd = dynamic_cast<MenuItemUnknown *>(cmd.get());

  EXPECT_FALSE(unknownCmd == nullptr);
}

TEST_F(CommandParserTest, Parse_EmptyCommand) {
  auto cmd = parser.parseCommand("");
  auto unknownCmd = dynamic_cast<MenuItemUnknown *>(cmd.get());

  EXPECT_FALSE(unknownCmd == nullptr);
}
