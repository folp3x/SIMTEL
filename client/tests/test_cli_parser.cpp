#include "app/cli_parser/cli_parser.h"

#include <filesystem>
#include <gtest/gtest.h>

#include "test_utils/args_splitter/args_splitter.h"
#include "test_utils/temp_file/temp_file.h"

class CliParserTest : public testing::Test {
protected:
  std::unique_ptr<client::CliParser> parser = client::CliParser::create();

  std::string msg = "";
  bool helpCalled = false;
};

TEST_F(CliParserTest, Parse_Help) {
  ArgsSplitter splitter{"./app --help"};
  bool parsed =
      parser->parse(splitter.argc(), splitter.argv(), msg, helpCalled);

  EXPECT_FALSE(parsed);
  EXPECT_TRUE(msg.size() > 0);
  EXPECT_TRUE(helpCalled);
}

TEST_F(CliParserTest, Parse_ConfigNoArg) {
  ArgsSplitter splitter{"./app --config"};

  bool parsed =
      parser->parse(splitter.argc(), splitter.argv(), msg, helpCalled);

  EXPECT_FALSE(parsed);
  EXPECT_FALSE(msg.empty());
  EXPECT_FALSE(helpCalled);
}

TEST_F(CliParserTest, Parse_ConfigRedundantArg) {
  ArgsSplitter splitter{"./app --config path path"};

  bool parsed =
      parser->parse(splitter.argc(), splitter.argv(), msg, helpCalled);

  EXPECT_FALSE(parsed);
  EXPECT_FALSE(msg.empty());
  EXPECT_FALSE(helpCalled);
}

TEST_F(CliParserTest, Parse_ConfigCorrectArg) {
  TempFile file{"./test.json", "[]"};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  ArgsSplitter splitter{"./app --config " + file.getPath()};

  bool parsed =
      parser->parse(splitter.argc(), splitter.argv(), msg, helpCalled);

  auto result = parser->getConfigFilePath();

  EXPECT_TRUE(parsed);
  EXPECT_TRUE(msg.empty());
  EXPECT_FALSE(helpCalled);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, file.getPath());
}

TEST_F(CliParserTest, Parse_ConfigInvalidArg) {
  ArgsSplitter splitter{"./app --config 123"};

  bool parsed =
      parser->parse(splitter.argc(), splitter.argv(), msg, helpCalled);

  parser->getConfigFilePath();

  EXPECT_FALSE(parsed);
  EXPECT_FALSE(msg.empty());
  EXPECT_FALSE(helpCalled);
}

TEST_F(CliParserTest, AllConfigOptsSet_AllSet) {
  ArgsSplitter splitter{"./app -a 127.0.0.1 -p 49152 -e 123456789012345 -i "
                        "543210987654321 -l 1.1"};

  bool parsed =
      parser->parse(splitter.argc(), splitter.argv(), msg, helpCalled);

  bool configOptsSet = parser->allConfigOptsSet();

  EXPECT_TRUE(parsed);
  EXPECT_TRUE(msg.empty());
  EXPECT_FALSE(helpCalled);
  EXPECT_TRUE(configOptsSet);
}

TEST_F(CliParserTest, AllConfigOptsSet_OneNotSetWithConfig) {
  TempFile file{"./test.json", "[]"};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  ArgsSplitter splitter{"./app -a 127.0.0.1 -p 49152 -e 123456789012345 -i "
                        "543210987654321 -k " +
                        file.getPath()};

  bool parsed =
      parser->parse(splitter.argc(), splitter.argv(), msg, helpCalled);

  bool configOptsSet = parser->allConfigOptsSet();

  EXPECT_TRUE(parsed);
  EXPECT_TRUE(msg.empty());
  EXPECT_FALSE(helpCalled);
  EXPECT_FALSE(configOptsSet);
}

TEST_F(CliParserTest, RedefineConfig_AllRedefined) {
  ArgsSplitter splitter{"./app -a 127.0.0.1 -p 49152 -e 123456789012345 -i "
                        "543210987654321 -l 1.1"};

  bool parsed =
      parser->parse(splitter.argc(), splitter.argv(), msg, helpCalled);

  client::Config config{};

  client::Config redefined = parser->redefineConfig(config);

  bool locRedefined = redefined.getLoc() == common::coords_t<float>{1.1};

  EXPECT_TRUE(parsed);
  EXPECT_TRUE(msg.empty());
  EXPECT_FALSE(helpCalled);
  EXPECT_TRUE(redefined.getIp() == "127.0.0.1");
  EXPECT_EQ(redefined.getPort(), 49152);
  EXPECT_TRUE(redefined.getImei() == "123456789012345");
  EXPECT_TRUE(redefined.getImsi() == "543210987654321");
  EXPECT_TRUE(locRedefined);
}
