#include "HumiditySensor.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/DHT.h"

using ::testing::ElementsAre;
using ::testing::Return;

using HumidityT = int8_t;  // cm

class HumiditySensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    pDhtMock = new DHTMock();
    pHs = new HumiditySensor(51, "HumiditySensor", *pDhtMock);
  }

  void TearDown() override {
    delete pHs;
    delete pDhtMock;
    releaseArduinoMock();
  }

  ArduinoMock* pArduinoMock;
  DHTMock* pDhtMock;
  HumiditySensor* pHs;
};

TEST_F(HumiditySensor_test,
       update_smallValueDiff_smallTimeDiff_shall_return_false) {
  EXPECT_CALL(*pDhtMock, readHumidity(false)).WillOnce(Return(1));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_FALSE(pHs->update());
}

TEST_F(HumiditySensor_test,
       update_smallValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDhtMock, readHumidity(false)).WillOnce(Return(1));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pHs->update());
}

TEST_F(HumiditySensor_test,
       update_largeValueDiff_smallTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDhtMock, readHumidity(false)).WillOnce(Return(2));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_TRUE(pHs->update());
}

TEST_F(HumiditySensor_test,
       update_largeValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDhtMock, readHumidity(false)).WillOnce(Return(2));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pHs->update());
}

TEST_F(HumiditySensor_test, getDiscoveryMsg) {
  uint8_t buf[18] = {};
  EXPECT_EQ(pHs->getDiscoveryMsg(buf), 18);
  // clang-format off
  EXPECT_THAT(
      buf,
      ElementsAre(
          51,
          static_cast<uint8_t>(BaseComponent::Type::sensor),
          static_cast<uint8_t>(SensorDeviceClass::humidity),
          static_cast<uint8_t>(Unit::Type::percent),
          (sizeof(HumidityT) << 4) | 0,
          4,
          0, static_cast<uint8_t>(Unit::Type::percent), (sizeof(HumidityT) << 4) | 0,
          1, static_cast<uint8_t>(Unit::Type::s), (sizeof(uint16_t) << 4) | 0,
          2, static_cast<uint8_t>(Unit::Type::s), (sizeof(uint16_t) << 4) | 0,
          3, static_cast<uint8_t>(Unit::Type::percent), (sizeof(HumidityT) << 4) | 0
  ));
  // clang-format on
}

TEST_F(HumiditySensor_test, getConfigItemValuesMsg) {
  uint8_t buf[12] = {};
  EXPECT_EQ(pHs->getConfigItemValuesMsg(buf), 12);
  // clang-format off
  EXPECT_THAT(
    buf,
    ElementsAre(
        51, 4,
        0, 2,
        1, highByte(60), lowByte(60),
        2, highByte(60), lowByte(60),
        3, 0
  ));
  // clang-format on
}

TEST_F(HumiditySensor_test, setConfigs_all_in_order) {
  uint8_t buf[] = {
      // clang-format off
      0, 100,
      1, highByte(1001), lowByte(1001),
      2, highByte(1002), lowByte(1002),
      3, 103
      // clang-format on
  };
  EXPECT_TRUE(pHs->setConfigs(4, buf));
  uint8_t expect_buf[12] = {};
  EXPECT_EQ(pHs->getConfigItemValuesMsg(expect_buf), 12);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
        51, 4,
        0, 100,
        1, highByte(1001), lowByte(1001),
        2, highByte(1002), lowByte(1002),
        3, 103
  ));
  // clang-format on
}

TEST_F(HumiditySensor_test, setConfigs_all_out_of_order) {
  uint8_t buf[] = {
      // clang-format off
      3, 23,
      2, highByte(2002), lowByte(2002),
      1, highByte(2001), lowByte(2001),
      0, 20
      // clang-format on
  };
  EXPECT_TRUE(pHs->setConfigs(4, buf));
  uint8_t expect_buf[12] = {};
  EXPECT_EQ(pHs->getConfigItemValuesMsg(expect_buf), 12);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
      51, 4,
      0, 20,
      1, highByte(2001), lowByte(2001),
      2, highByte(2002), lowByte(2002),
      3, 23
  ));
  // clang-format on
}

TEST_F(HumiditySensor_test, setConfigs_one) {
  uint8_t buf[] = {
      // clang-format off
      3, 33
      // clang-format on
  };
  EXPECT_TRUE(pHs->setConfigs(1, buf));
  uint8_t expect_buf[12] = {};
  EXPECT_EQ(pHs->getConfigItemValuesMsg(expect_buf), 12);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
        51, 4,
        0, 2,
        1, highByte(60), lowByte(60),
        2, highByte(60), lowByte(60),
        3, 33
  ));
  // clang-format on
}

TEST_F(HumiditySensor_test, setConfigs_too_many) {
  uint8_t buf[] = {
      // clang-format off
      0, 100,
      1, highByte(1001), lowByte(1001),
      2, highByte(1002), lowByte(1002),
      3, 103,
      4, highByte(1004), lowByte(1004)
      // clang-format on
  };
  EXPECT_FALSE(pHs->setConfigs(5, buf));
}

TEST_F(HumiditySensor_test, setConfigs_out_of_range) {
  uint8_t buf[] = {
      // clang-format off
      4, highByte(3004), lowByte(3004)
      // clang-format on
  };
  EXPECT_FALSE(pHs->setConfigs(1, buf));
}

TEST_F(HumiditySensor_test,
       update_largeValueDiff_largeTimeDiff_withConfigsZero_shall_return_false) {
  EXPECT_CALL(*pDhtMock, readHumidity(false)).WillOnce(Return(2));
  EXPECT_CALL(*pArduinoMock, millis()).Times(0);
  uint8_t buf[] = {
      // clang-format off
      0, 0, 0,
      1, 0, 0,
      2, 0, 0,
      3, 0, 0
      // clang-format on
  };
  EXPECT_TRUE(pHs->setConfigs(4, buf));
  EXPECT_FALSE(pHs->update());
}
