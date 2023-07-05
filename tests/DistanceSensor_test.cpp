#include "DistanceSensor.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/Arduino.h"
#include "mocks/NewPing.h"

#define SONAR_TRIGGER_PIN 7
#define SONAR_ECHO_PIN 6
#define SONAR_MAX_DISTANCE_CM 300

using ::testing::ElementsAre;
using ::testing::Return;
using DistanceT = int16_t;  // cm

class DistanceSensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    pSonarMock = new NewPingMock();
    pDs = new DistanceSensor(7, "DistanceSensor", *pSonarMock);
  }

  void TearDown() override {
    delete pDs;
    delete pSonarMock;
    releaseArduinoMock();
  }

  ArduinoMock* pArduinoMock;
  NewPingMock* pSonarMock;
  DistanceSensor* pDs;
};

TEST_F(DistanceSensor_test,
       update_smallValueDiff_smallTimeDiff_shall_return_false) {
  EXPECT_CALL(*pSonarMock, ping_cm(0)).WillOnce(Return(9));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_FALSE(pDs->update());
}

TEST_F(DistanceSensor_test,
       update_smallValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pSonarMock, ping_cm(0)).WillOnce(Return(9));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pDs->update());
}

TEST_F(DistanceSensor_test,
       update_largeValueDiff_smallTimeDiff_shall_return_true) {
  EXPECT_CALL(*pSonarMock, ping_cm(0)).WillOnce(Return(10));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_TRUE(pDs->update());
}

TEST_F(DistanceSensor_test,
       update_largeValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pSonarMock, ping_cm(0)).WillOnce(Return(10));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pDs->update());
}

// TODO: Add test of update where configs are 0

TEST_F(DistanceSensor_test, getDiscoveryMsg) {
  uint8_t buf[15] = {};
  EXPECT_EQ(pDs->getDiscoveryMsg(buf), 15);
  // clang-format off
  EXPECT_THAT(
      buf,
      ElementsAre(
          7,
          static_cast<uint8_t>(BaseComponent::Type::sensor),
          static_cast<uint8_t>(SensorDeviceClass::distance),
          static_cast<uint8_t>(Unit::Type::cm),
          (sizeof(DistanceT) << 4) | 0,
          3,
          0, static_cast<uint8_t>(Unit::Type::cm), (sizeof(DistanceT) << 4) | 0,
          1, static_cast<uint8_t>(Unit::Type::s), (sizeof(uint16_t) << 4) | 0,
          2, static_cast<uint8_t>(Unit::Type::s), (sizeof(uint16_t) << 4) | 0
  ));
  // clang-format on
}

TEST_F(DistanceSensor_test, getConfigItemValuesMsg) {
  uint8_t buf[11] = {};
  EXPECT_EQ(pDs->getConfigItemValuesMsg(buf), 11);
  // clang-format off
  EXPECT_THAT(
    buf,
    ElementsAre(
        7, 3,
        0, highByte(0), lowByte(10),
        1, highByte(0), lowByte(60),
        2, highByte(0), lowByte(60)
  ));
  // clang-format on
}

TEST_F(DistanceSensor_test, setConfigs_all_in_order) {
  uint8_t buf[] = {
      // clang-format off
    0, highByte(1000), lowByte(1000),
    1, highByte(1001), lowByte(1001),
    2, highByte(1002), lowByte(1002)
      // clang-format on
  };
  EXPECT_TRUE(pDs->setConfigs(3, buf));
  uint8_t expect_buf[11] = {};
  EXPECT_EQ(pDs->getConfigItemValuesMsg(expect_buf), 11);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
        7, 3,
        0, highByte(1000), lowByte(1000),
        1, highByte(1001), lowByte(1001),
        2, highByte(1002), lowByte(1002)
  ));
  // clang-format on
}

TEST_F(DistanceSensor_test, setConfigs_all_out_of_order) {
  uint8_t buf[] = {
      // clang-format off
      2, highByte(2002), lowByte(2002),
      1, highByte(2001), lowByte(2001),
      0, highByte(2000), lowByte(2000)
      // clang-format on
  };
  EXPECT_TRUE(pDs->setConfigs(3, buf));
  uint8_t expect_buf[11] = {};
  EXPECT_EQ(pDs->getConfigItemValuesMsg(expect_buf), 11);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
        7, 3,
        0, highByte(2000), lowByte(2000),
        1, highByte(2001), lowByte(2001),
        2, highByte(2002), lowByte(2002)
  ));
  // clang-format on
}

TEST_F(DistanceSensor_test, setConfigs_one) {
  uint8_t buf[] = {
      // clang-format off
      2, highByte(3002), lowByte(3002)
      // clang-format on
  };
  EXPECT_TRUE(pDs->setConfigs(1, buf));
  uint8_t expect_buf[11] = {};
  EXPECT_EQ(pDs->getConfigItemValuesMsg(expect_buf), 11);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
        7, 3,
        0, highByte(0), lowByte(10),
        1, highByte(0), lowByte(60),
        2, highByte(3002), lowByte(3002)
  ));
  // clang-format on
}

TEST_F(DistanceSensor_test, setConfigs_too_many) {
  uint8_t buf[] = {
      // clang-format off
      0, highByte(1000), lowByte(1000),
      1, highByte(1001), lowByte(1001),
      2, highByte(1002), lowByte(1002),
      3, highByte(1003), lowByte(1003)
      // clang-format on
  };
  EXPECT_FALSE(pDs->setConfigs(4, buf));
}

TEST_F(DistanceSensor_test, setConfigs_out_of_range) {
  uint8_t buf[] = {
      // clang-format off
      4, highByte(3004), lowByte(3004)
      // clang-format on
  };
  EXPECT_FALSE(pDs->setConfigs(1, buf));
}

TEST_F(DistanceSensor_test,
       update_largeValueDiff_largeTimeDiff_withConfigsZero_shall_return_false) {
  EXPECT_CALL(*pSonarMock, ping_cm(0)).WillOnce(Return(10));
  EXPECT_CALL(*pArduinoMock, millis()).Times(0);
  uint8_t buf[] = {
      // clang-format off
      0, 0, 0,
      1, 0, 0,
      2, 0, 0
      // clang-format on
  };
  EXPECT_TRUE(pDs->setConfigs(3, buf));
  EXPECT_FALSE(pDs->update());
}
