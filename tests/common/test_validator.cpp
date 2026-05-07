#include "common/validator/validator.h"

#include <filesystem>
#include <gtest/gtest.h>

#include "tests/utils/temp_file.h"

class ValidatorTest : public ::testing::Test {
protected:
  static constexpr int IMEI_LEN = 15;
  static constexpr int IMSI_LEN = 15;
};

// isCorrectIP
TEST_F(ValidatorTest, IsCorrectIP_LowByteLessThanMin) {
  // 0.0.0.0
  uint32_t ip = 0;
  EXPECT_FALSE(Validator::isCorrectIP(ip).empty());
}

TEST_F(ValidatorTest, IsCorrectIP_LowByteGreaterThenMax) {
  // 0.0.0.255
  uint32_t ip = 255 << (3 * 8);
  EXPECT_FALSE(Validator::isCorrectIP(ip).empty());
}

TEST_F(ValidatorTest, IsCorrectIP_Correct) {
  // 0.0.0.253
  uint32_t ip = 253 << (3 * 8);
  EXPECT_TRUE(Validator::isCorrectIP(ip).empty());
}

// isCorrectPort
TEST_F(ValidatorTest, IsCorrectPort_LessThanMin) {
  int port = 0;
  EXPECT_FALSE(Validator::isCorrectPort(port).empty());
}

TEST_F(ValidatorTest, IsCorrectPort_GreaterThanMax) {
  int port = 70000;
  EXPECT_FALSE(Validator::isCorrectPort(port).empty());
}

TEST_F(ValidatorTest, IsCorrectPort_Correct) {
  int port = 60000;
  EXPECT_TRUE(Validator::isCorrectPort(port).empty());
}

// isCorrectIMEI
TEST_F(ValidatorTest, IsCorrectImei_Empty) {
  std::string imei = "";
  EXPECT_FALSE(Validator::isCorrectIMEI(imei).empty());
}

TEST_F(ValidatorTest, IsCorrectImei_LengthLess) {
  std::string imei = "1";
  EXPECT_FALSE(Validator::isCorrectIMEI(imei).empty());
}

TEST_F(ValidatorTest, IsCorrectImei_LengthGreater) {
  std::string imei = std::string(IMEI_LEN + 1, '1');
  EXPECT_FALSE(Validator::isCorrectIMEI(imei).empty());
}

TEST_F(ValidatorTest, IsCorrectImei_WithLetters) {
  std::string imei = std::string(IMEI_LEN - 1, '1') + "A";
  EXPECT_FALSE(Validator::isCorrectIMEI(imei).empty());
}

TEST_F(ValidatorTest, IsCorrectImei_Correct) {
  std::string imei = std::string(IMEI_LEN, '1');
  EXPECT_TRUE(Validator::isCorrectIMEI(imei).empty());
}

// isCorrectIMSI
TEST_F(ValidatorTest, IsCorrectImsi_Empty) {
  std::string imsi = "";
  EXPECT_FALSE(Validator::isCorrectIMSI(imsi).empty());
}

TEST_F(ValidatorTest, IsCorrectImsi_LengthLess) {
  std::string imsi = "1";
  EXPECT_FALSE(Validator::isCorrectIMSI(imsi).empty());
}

TEST_F(ValidatorTest, IsCorrectImsi_LengthGreater) {
  std::string imsi = std::string(IMSI_LEN + 1, '1');
  EXPECT_FALSE(Validator::isCorrectIMSI(imsi).empty());
}

TEST_F(ValidatorTest, IsCorrectImsi_WithLetters) {
  std::string imsi = std::string(IMSI_LEN - 1, '1') + "A";
  EXPECT_FALSE(Validator::isCorrectIMSI(imsi).empty());
}

TEST_F(ValidatorTest, IsCorrectImsi_Correct) {
  std::string imei = std::string(IMSI_LEN, '1');
  EXPECT_TRUE(Validator::isCorrectIMSI(imei).empty());
}

// isCorrectConfigFilePath
TEST_F(ValidatorTest, IsCorrectConfigPath_Empty) {
  std::string path = "";
  EXPECT_FALSE(Validator::isCorrectConfigPath(path).empty());
}

TEST_F(ValidatorTest, IsCorrectConfigPath_NonJsonShortExt) {
  std::string path = "file.c";
  EXPECT_FALSE(Validator::isCorrectConfigPath(path).empty());
}

TEST_F(ValidatorTest, IsCorrectConfigPath_NonJsonLongExt) {
  std::string path = "file.config";
  EXPECT_FALSE(Validator::isCorrectConfigPath(path).empty());
}

TEST_F(ValidatorTest, IsCorrectConfigPath_FileNotExists) {
  std::string path = "123.json";
  if (std::filesystem::exists(path))
    GTEST_SKIP() << "Tested as non existing file exists, skipping test";

  EXPECT_FALSE(Validator::isCorrectConfigPath(path).empty());
}

TEST_F(ValidatorTest, IsCorrectConfigPath_FileExists) {
  TempFile file("test.json", "");

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  EXPECT_TRUE(Validator::isCorrectConfigPath(file.getPath()).empty());
}

// isCorrectNodesFilePath
TEST_F(ValidatorTest, IsCorrectNodesPath_Empty) {
  std::string path = "";
  EXPECT_FALSE(Validator::isCorrectNodesPath(path).empty());
}

TEST_F(ValidatorTest, IsCorrectNodesPath_NonJsonShortExt) {
  std::string path = "file.c";
  EXPECT_FALSE(Validator::isCorrectNodesPath(path).empty());
}

TEST_F(ValidatorTest, IsCorrectNodesPath_NonJsonLongExt) {
  std::string path = "file.config";
  EXPECT_FALSE(Validator::isCorrectNodesPath(path).empty());
}

TEST_F(ValidatorTest, IsCorrectNodesPath_FileNotExists) {
  std::string path = "123.json";
  if (std::filesystem::exists(path))
    GTEST_SKIP() << "Tested as non existing file exists, skipping test";

  EXPECT_FALSE(Validator::isCorrectNodesPath(path).empty());
}

TEST_F(ValidatorTest, IsCorrectNodesPath_FileExists) {
  TempFile file("test.json", "");

  if (!std::filesystem::exists(file.getPath()))
    GTEST_SKIP() << "Error creating temp file, skipping test";

  EXPECT_TRUE(Validator::isCorrectNodesPath(file.getPath()).empty());
}
