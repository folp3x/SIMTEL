#include "app/config/config/config.h"

#include <filesystem>
#include <gtest/gtest.h>

#include "test_utils/temp_file/temp_file.h"

class ConfigParseTest : public testing::Test {
protected:
  nlohmann::json getValidJson() {
    return nlohmann::json{{"ip", "127.0.0.1"},
                          {"port", 49152},
                          {"imei", "123456789012345"},
                          {"imsi", "543210987654321"},
                          {"loc", {1.1}},
                          {"addressBookFilePath", "address_book.json"}};
  }
};

TEST_F(ConfigParseTest, Parse_NoPortValue) {
  nlohmann::json json{};
  TempFile file{"test.json", json.dump()};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  client::Config config;
  auto error = config.fromJsonFile(file.getPath());

  EXPECT_TRUE(error.has_value());
}

TEST_F(ConfigParseTest, Parse_NonNumericPortValue) {
  nlohmann::json json = {"port", "65535"};
  TempFile file{"test.json", json.dump()};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  client::Config config;
  auto error = config.fromJsonFile(file.getPath());

  EXPECT_TRUE(error.has_value());
}

TEST_F(ConfigParseTest, Parse_NonIntegerPortValue) {
  nlohmann::json json = {"port", 65535.1};
  TempFile file{"test.json", json.dump()};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  client::Config config;
  auto error = config.fromJsonFile(file.getPath());

  EXPECT_TRUE(error.has_value());
}

TEST_F(ConfigParseTest, Parse_CorrectJson) {
  nlohmann::json json = getValidJson();
  TempFile configFile{"test.json", json.dump()};

  TempFile addressBookFile{"address_book.json", "[]"};

  if (!std::filesystem::exists(configFile.getPath()))
    GTEST_SKIP() << "Error creating temp config file, skipping test";

  if (!std::filesystem::exists(addressBookFile.getPath()))
    GTEST_SKIP() << "Error creating temp address book file, skipping test";

  client::Config config;
  auto error = config.fromJsonFile(configFile.getPath());

  ASSERT_FALSE(error.has_value());

  bool locParsed = config.getLoc() == common::coords_t<float>{1.1};

  EXPECT_TRUE(config.getIp() == "127.0.0.1");
  EXPECT_EQ(config.getPort(), 49152);
  EXPECT_TRUE(config.getImei() == "123456789012345");
  EXPECT_TRUE(config.getImsi() == "543210987654321");
  EXPECT_TRUE(config.getAddressBookFilePath() == addressBookFile.getPath());
  EXPECT_TRUE(locParsed);
}

TEST_F(ConfigParseTest, Parse_NoFile) {
  client::Config config;
  auto error = config.fromJsonFile("123");

  EXPECT_TRUE(error.has_value());
}

TEST_F(ConfigParseTest, Parse_EmptyJson) {
  TempFile file{"test.json", ""};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  client::Config config;
  auto error = config.fromJsonFile("123");

  EXPECT_TRUE(error.has_value());
}

TEST_F(ConfigParseTest, Parse_InvalidJson) {
  nlohmann::json json = getValidJson();
  TempFile file{"test.json", json.dump().substr(1)};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  client::Config config;
  auto error = config.fromJsonFile("123");

  EXPECT_TRUE(error.has_value());
}

TEST_F(ConfigParseTest, Parse_MissingIP) {
  nlohmann::json json = getValidJson();
  json.erase("ip");
  TempFile file{"test.json", json.dump()};

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  client::Config config;
  auto error = config.fromJsonFile("123");

  EXPECT_TRUE(error.has_value());
}
