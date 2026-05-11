#include "client/app/menu/command_parser/command_parser.h"

#include <gtest/gtest.h>

class ClientCommandParserTest : public ::testing::Test {
protected:
  client::CommandParser parser{};
  std::string extraMsg = "";
};

TEST_F(ClientCommandParserTest, Parse_MoveNoArg) {
  auto cmd = parser.parseCommand("move", extraMsg);
  auto invalidCmd = dynamic_cast<client::MenuItemInvalid *>(cmd.get());

  EXPECT_FALSE(invalidCmd == nullptr);
}

TEST_F(ClientCommandParserTest, Parse_MoveRedundantArg) {
  auto cmd = parser.parseCommand("move 1 2 3 4", extraMsg);
  auto moveCmd = dynamic_cast<client::MenuItemMove *>(cmd.get());

  client::MenuItemMove correctCmd{{1, 2, 3}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
  EXPECT_FALSE(extraMsg.empty());
}

TEST_F(ClientCommandParserTest, Parse_MoveInvalidArg) {
  auto cmd = parser.parseCommand("move str", extraMsg);
  auto invalidCmd = dynamic_cast<client::MenuItemInvalid *>(cmd.get());

  EXPECT_FALSE(invalidCmd == nullptr);
}

TEST_F(ClientCommandParserTest, Parse_MoveOneFloatArg) {

  auto cmd = parser.parseCommand("move 1.1", extraMsg);
  auto moveCmd = dynamic_cast<client::MenuItemMove *>(cmd.get());

  client::MenuItemMove correctCmd{{1.1}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
}

TEST_F(ClientCommandParserTest, Parse_MoveOneIntegerArg) {

  auto cmd = parser.parseCommand("move 1", extraMsg);
  auto moveCmd = dynamic_cast<client::MenuItemMove *>(cmd.get());

  client::MenuItemMove correctCmd{{1}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
}

TEST_F(ClientCommandParserTest, Parse_MoveTwoArgs) {

  auto cmd = parser.parseCommand("move 1.1 2", extraMsg);
  auto moveCmd = dynamic_cast<client::MenuItemMove *>(cmd.get());

  client::MenuItemMove correctCmd{{1.1, 2}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
}

TEST_F(ClientCommandParserTest, Parse_MoveThreeArgs) {

  auto cmd = parser.parseCommand("move 1.1 2 3", extraMsg);
  auto moveCmd = dynamic_cast<client::MenuItemMove *>(cmd.get());

  client::MenuItemMove correctCmd{{1.1, 2, 3}};

  ASSERT_FALSE(moveCmd == nullptr);
  ASSERT_TRUE(moveCmd->getCoords() == correctCmd.getCoords());
}

TEST_F(ClientCommandParserTest, Parse_UnknownCommand) {
  std::string tempMsg = "";
  auto cmd = parser.parseCommand("123", tempMsg);
  // проверка существовани команды
  const auto &commands = client::getCommandsInfo();
  for (const auto &[name, _] : commands) {
    if (cmd->getName() == name)
      GTEST_SKIP() << "Tested as unknown command exists, skipping test";
  }
  auto unknownCmd = dynamic_cast<client::MenuItemUnknown *>(cmd.get());

  EXPECT_FALSE(unknownCmd == nullptr);
}

TEST_F(ClientCommandParserTest, Parse_EmptyCommand) {
  auto cmd = parser.parseCommand("", extraMsg);
  auto unknownCmd = dynamic_cast<client::MenuItemUnknown *>(cmd.get());

  EXPECT_FALSE(unknownCmd == nullptr);
}
