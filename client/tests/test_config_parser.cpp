#include "client/app/config/config_parser/config_parser.h"

#include <filesystem>
#include <gtest/gtest.h>

#include "test_utils/temp_file/temp_file.h"

class ConfigParserTest : public ::testing::Test {
protected:
  std::unique_ptr<client::ConfigParser> parser = client::ConfigParser::create();

  std::string msg = "";
  bool helpCalled = false;

  nlohmann::json getValidJson() {
    return nlohmann::json{{"ip", "127.0.0.1"},
                          {"port", 49152},
                          {"imei", "123456789012345"},
                          {"imsi", "543210987654321"},
                          {"loc", {1.1}},
                          {"addressBookFilePath", "address_book.json"}};
  }
};

TEST_F(ConfigParserTest, Parse_NoPortValue) {
  nlohmann::json json = {};
  TempFile file{"test.json", json.dump()};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  auto result = parser->parse(file.getPath());

  EXPECT_FALSE(result.has_value());
}

TEST_F(ConfigParserTest, Parse_NonNumericPortValue) {
  nlohmann::json json = {"port", "65535"};
  TempFile file{"test.json", json.dump()};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  auto result = parser->parse(file.getPath());

  EXPECT_FALSE(result.has_value());
}

TEST_F(ConfigParserTest, Parse_NonIntegerPortValue) {
  nlohmann::json json = {"port", 65535.1};
  TempFile file{"test.json", json.dump()};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  auto result = parser->parse(file.getPath());

  EXPECT_FALSE(result.has_value());
}

TEST_F(ConfigParserTest, Parse_CorrectJson) {
  nlohmann::json json = getValidJson();
  TempFile configFile{"test.json", json.dump()};

  TempFile addressBookFile{"address_book.json", "[]"};

  if (!std::filesystem::exists(configFile.getPath()))
    GTEST_SKIP() << "Error creating temp config file, skipping test";

  if (!std::filesystem::exists(addressBookFile.getPath()))
    GTEST_SKIP() << "Error creating temp address book file, skipping test";

  auto result = parser->parse(configFile.getPath());

  ASSERT_TRUE(result.has_value());

  client::Config config = *result;
  bool locParsed = config.getLoc() == common::coords_t<float>{1.1};

  EXPECT_TRUE(config.getIp() == "127.0.0.1");
  EXPECT_EQ(config.getPort(), 49152);
  EXPECT_TRUE(config.getImei() == "123456789012345");
  EXPECT_TRUE(config.getImsi() == "543210987654321");
  EXPECT_TRUE(config.getAddressBookFilePath() == addressBookFile.getPath());
  EXPECT_TRUE(locParsed);
}

TEST_F(ConfigParserTest, Parse_NoFile) {
  auto result = parser->parse("123");

  EXPECT_FALSE(result.has_value());
}

TEST_F(ConfigParserTest, Parse_EmptyJson) {
  nlohmann::json json = getValidJson();
  TempFile file{"test.json", ""};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  auto result = parser->parse(file.getPath());

  EXPECT_FALSE(result.has_value());
}

TEST_F(ConfigParserTest, Parse_InvalidJson) {
  nlohmann::json json = getValidJson();
  TempFile file{"test.json", json.dump().substr(1)};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  auto result = parser->parse(file.getPath());

  EXPECT_FALSE(result.has_value());
}

TEST_F(ConfigParserTest, Parse_MissingIP) {
  nlohmann::json json = getValidJson();
  json.erase("ip");
  TempFile file{"test.json", json.dump()};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  auto result = parser->parse(file.getPath());

  EXPECT_FALSE(result.has_value());
}
