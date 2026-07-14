#include "common/network/network_address/network_address.h"

#include <gtest/gtest.h>

class NetworkAddressTest : public testing::Test {
protected:
  // 127.0.0.1
  static constexpr uint32_t TestIp = (1) | (127 << 3 * 8);
  static constexpr uint16_t TestPort = 49152;
};

TEST_F(NetworkAddressTest, Constructor_int_int) {
  common::NetworkAddress addr(TestIp, TestPort);
  EXPECT_EQ(addr.getIp(), TestIp);
  EXPECT_EQ(addr.getPort(), TestPort);
}

TEST_F(NetworkAddressTest, Constructor_str_str) {
  common::NetworkAddress addr("127.0.0.1", "49152");
  EXPECT_EQ(addr.getIp(), TestIp);
  EXPECT_EQ(addr.getPort(), TestPort);
}

TEST_F(NetworkAddressTest, Constructor_strIP_intPort) {
  common::NetworkAddress addr("127.0.0.1", 49152);
  EXPECT_EQ(addr.getIp(), TestIp);
  EXPECT_EQ(addr.getPort(), TestPort);
}

TEST_F(NetworkAddressTest, Constructor_FullAddress) {
  common::NetworkAddress addr("127.0.0.1:49152");
  EXPECT_EQ(addr.getIp(), TestIp);
  EXPECT_EQ(addr.getPort(), TestPort);
}

TEST_F(NetworkAddressTest, FromStr_Correct) {
  common::NetworkAddress addr =
      common::NetworkAddress::fromStr("127.0.0.1:49152");
  EXPECT_EQ(addr.getIp(), TestIp);
  EXPECT_EQ(addr.getPort(), TestPort);
}

TEST_F(NetworkAddressTest, FromStr_NoPort) {
  EXPECT_THROW(common::NetworkAddress::fromStr("127.0.0.1"),
               std::invalid_argument);
}

TEST_F(NetworkAddressTest, FromStr_InvalidDelimiter) {
  EXPECT_THROW(common::NetworkAddress::fromStr("127.0.0.1-49152"),
               std::invalid_argument);
}

TEST_F(NetworkAddressTest, FromStr_InvalidIP) {
  EXPECT_THROW(common::NetworkAddress::fromStr("invalid:49152"),
               std::invalid_argument);
}

TEST_F(NetworkAddressTest, FromStr_InvalidPort) {
  EXPECT_THROW(common::NetworkAddress::fromStr("127.0.0.1:invalid"),
               std::invalid_argument);
}
