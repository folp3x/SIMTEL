#include "common/validator/validator.h"

#include <filesystem>
#include <gtest/gtest.h>

#include "tests/utils/temp_file/temp_file.h"

class CommonValidatorTest : public ::testing::Test {
protected:
  static constexpr int MIN_IMEI_LENGTH = 1;
  static constexpr int MAX_IMEI_LENGTH = 15;
  static constexpr int MIN_IMSI_LENGTH = 6;
  static constexpr int MAX_IMSI_LENGTH = 15;
};

// isCorrectIP
TEST_F(CommonValidatorTest, IsCorrectIP_LowByteLessThanMin) {
  // 0.0.0.0
  uint32_t ip = 0;
  EXPECT_FALSE(common::Validator::isCorrectIP(ip).empty());
}

TEST_F(CommonValidatorTest, IsCorrectIP_LowByteGreaterThenMax) {
  // 0.0.0.255
  uint32_t ip = 255 << (3 * 8);
  EXPECT_FALSE(common::Validator::isCorrectIP(ip).empty());
}

TEST_F(CommonValidatorTest, IsCorrectIP_Correct) {
  // 0.0.0.253
  uint32_t ip = 253 << (3 * 8);
  EXPECT_TRUE(common::Validator::isCorrectIP(ip).empty());
}

// isCorrectPort
TEST_F(CommonValidatorTest, IsCorrectPort_LessThanMin) {
  int port = 0;
  EXPECT_FALSE(common::Validator::isCorrectPort(port).empty());
}

TEST_F(CommonValidatorTest, IsCorrectPort_GreaterThanMax) {
  int port = 70000;
  EXPECT_FALSE(common::Validator::isCorrectPort(port).empty());
}

TEST_F(CommonValidatorTest, IsCorrectPort_Correct) {
  int port = 60000;
  EXPECT_TRUE(common::Validator::isCorrectPort(port).empty());
}

// isCorrectIMEI
TEST_F(CommonValidatorTest, IsCorrectImei_Empty) {
  common::imei_t imei = "";
  EXPECT_FALSE(common::Validator::isCorrectIMEI(imei).empty());
}

TEST_F(CommonValidatorTest, IsCorrectImei_LengthGreater) {
  common::imei_t imei = std::string(MAX_IMEI_LENGTH + 1, '1');
  EXPECT_FALSE(common::Validator::isCorrectIMEI(imei).empty());
}

TEST_F(CommonValidatorTest, IsCorrectImei_WithLetters) {
  common::imei_t imei = std::string(MAX_IMEI_LENGTH - 1, '1') + "A";
  EXPECT_FALSE(common::Validator::isCorrectIMEI(imei).empty());
}

TEST_F(CommonValidatorTest, IsCorrectImei_Correct) {
  common::imei_t imei = std::string(MAX_IMEI_LENGTH - 1, '1');
  EXPECT_TRUE(common::Validator::isCorrectIMEI(imei).empty());
}

// isCorrectIMSI
TEST_F(CommonValidatorTest, IsCorrectImsi_Empty) {
  common::imsi_t imsi = "";
  EXPECT_FALSE(common::Validator::isCorrectIMSI(imsi).empty());
}

TEST_F(CommonValidatorTest, IsCorrectImsi_LengthLess) {
  common::imsi_t imsi = std::string(MIN_IMSI_LENGTH - 1, '1');
  EXPECT_FALSE(common::Validator::isCorrectIMSI(imsi).empty());
}

TEST_F(CommonValidatorTest, IsCorrectImsi_LengthGreater) {
  common::imsi_t imsi = std::string(MAX_IMSI_LENGTH + 1, '1');
  EXPECT_FALSE(common::Validator::isCorrectIMSI(imsi).empty());
}

TEST_F(CommonValidatorTest, IsCorrectImsi_WithLetters) {
  common::imsi_t imsi = std::string(MAX_IMSI_LENGTH - 1, '1') + "A";
  EXPECT_FALSE(common::Validator::isCorrectIMSI(imsi).empty());
}

TEST_F(CommonValidatorTest, IsCorrectImsi_Correct) {
  common::imei_t imei = std::string(MIN_IMSI_LENGTH + 1, '1');
  EXPECT_TRUE(common::Validator::isCorrectIMSI(imei).empty());
}

// isCorrectConfigFilePath
TEST_F(CommonValidatorTest, IsCorrectConfigPath_Empty) {
  std::string path = "";
  EXPECT_FALSE(common::Validator::isCorrectConfigPath(path).empty());
}

TEST_F(CommonValidatorTest, IsCorrectConfigPath_NonJsonShortExt) {
  std::string path = "file.c";
  EXPECT_FALSE(common::Validator::isCorrectConfigPath(path).empty());
}

TEST_F(CommonValidatorTest, IsCorrectConfigPath_NonJsonLongExt) {
  std::string path = "file.config";
  EXPECT_FALSE(common::Validator::isCorrectConfigPath(path).empty());
}

TEST_F(CommonValidatorTest, IsCorrectConfigPath_FileNotExists) {
  std::string path = "123.json";
  if (std::filesystem::exists(path))
    GTEST_SKIP() << "Tested as non existing file exists, skipping test";

  EXPECT_FALSE(common::Validator::isCorrectConfigPath(path).empty());
}

TEST_F(CommonValidatorTest, IsCorrectConfigPath_FileExists) {
  TempFile file("test.json", "");

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  EXPECT_TRUE(common::Validator::isCorrectConfigPath(file.getPath()).empty());
}
