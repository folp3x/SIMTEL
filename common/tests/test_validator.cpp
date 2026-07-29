#include "common/validator/validator.h"

#include <filesystem>
#include <gtest/gtest.h>

#include "test_utils/temp_file/temp_file.h"

class ValidatorTest : public testing::Test {
protected:
  static constexpr size_t MaxImeiLength = 15;
  static constexpr size_t MinImsiLength = 6;
  static constexpr size_t MaxImsiLength = 15;
};

// isCorrectIp
TEST_F(ValidatorTest, IsCorrectIP_LowByteLessThanMin) {
  // 0.0.0.0
  uint32_t ip = 0;
  EXPECT_FALSE(common::Validator::isCorrectIp(ip).empty());
}

TEST_F(ValidatorTest, IsCorrectIP_LowByteGreaterThanMax) {
  // 0.0.0.255
  uint32_t ip = 255;
  EXPECT_FALSE(common::Validator::isCorrectIp(ip).empty());
}

TEST_F(ValidatorTest, IsCorrectIP_Correct) {
  // 0.0.0.253
  uint32_t ip = 253;
  EXPECT_TRUE(common::Validator::isCorrectIp(ip).empty());
}

// isCorrectPort
TEST_F(ValidatorTest, IsCorrectPort_LessThanMin) {
  int port = 0;
  EXPECT_FALSE(common::Validator::isCorrectPort(port).empty());
}

TEST_F(ValidatorTest, IsCorrectPort_GreaterThanMax) {
  int port = 70000;
  EXPECT_FALSE(common::Validator::isCorrectPort(port).empty());
}

TEST_F(ValidatorTest, IsCorrectPort_Correct) {
  int port = 60000;
  EXPECT_TRUE(common::Validator::isCorrectPort(port).empty());
}

// isCorrectImei
TEST_F(ValidatorTest, IsCorrectImei_Empty) {
  common::imei_t imei = "";
  EXPECT_FALSE(common::Validator::isCorrectImei(imei).empty());
}

TEST_F(ValidatorTest, IsCorrectImei_LengthGreater) {
  common::imei_t imei = std::string(MaxImeiLength + 1, '1');
  EXPECT_FALSE(common::Validator::isCorrectImei(imei).empty());
}

TEST_F(ValidatorTest, IsCorrectImei_WithLetters) {
  common::imei_t imei = std::string(MaxImeiLength - 1, '1') + "A";
  EXPECT_FALSE(common::Validator::isCorrectImei(imei).empty());
}

TEST_F(ValidatorTest, IsCorrectImei_Correct) {
  common::imei_t imei = std::string(MaxImeiLength - 1, '1');
  EXPECT_TRUE(common::Validator::isCorrectImei(imei).empty());
}

// isCorrectIMSI
TEST_F(ValidatorTest, IsCorrectImsi_Empty) {
  common::imsi_t imsi = "";
  EXPECT_FALSE(common::Validator::isCorrectImsi(imsi).empty());
}

TEST_F(ValidatorTest, IsCorrectImsi_LengthLess) {
  common::imsi_t imsi = std::string(MinImsiLength - 1, '1');
  EXPECT_FALSE(common::Validator::isCorrectImsi(imsi).empty());
}

TEST_F(ValidatorTest, IsCorrectImsi_LengthGreater) {
  common::imsi_t imsi = std::string(MaxImsiLength + 1, '1');
  EXPECT_FALSE(common::Validator::isCorrectImsi(imsi).empty());
}

TEST_F(ValidatorTest, IsCorrectImsi_WithLetters) {
  common::imsi_t imsi = std::string(MaxImsiLength - 1, '1') + "A";
  EXPECT_FALSE(common::Validator::isCorrectImsi(imsi).empty());
}

TEST_F(ValidatorTest, IsCorrectImsi_Correct) {
  common::imei_t imei = std::string(MinImsiLength + 1, '1');
  EXPECT_TRUE(common::Validator::isCorrectImsi(imei).empty());
}

// isCorrectConfigFilePath
TEST_F(ValidatorTest, IsCorrectConfigPath_Empty) {
  std::string path = "";
  EXPECT_FALSE(common::Validator::jsonFilePathExists(path, "config").empty());
}

TEST_F(ValidatorTest, IsCorrectConfigPath_NonJsonShortExt) {
  std::string path = "file.c";
  EXPECT_FALSE(common::Validator::jsonFilePathExists(path, "config").empty());
}

TEST_F(ValidatorTest, IsCorrectConfigPath_NonJsonLongExt) {
  std::string path = "file.config";
  EXPECT_FALSE(common::Validator::jsonFilePathExists(path, "config").empty());
}

TEST_F(ValidatorTest, IsCorrectConfigPath_FileNotExists) {
  std::string path = "123.json";
  if (std::filesystem::exists(path))
    GTEST_SKIP() << "Tested as non existing file exists, skipping test";

  EXPECT_FALSE(common::Validator::jsonFilePathExists(path, "config").empty());
}

TEST_F(ValidatorTest, IsCorrectConfigPath_FileExists) {
  TempFile file("test.json", "");

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  EXPECT_TRUE(
      common::Validator::jsonFilePathExists(file.getPath(), "config").empty());
}
