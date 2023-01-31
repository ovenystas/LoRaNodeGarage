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
  EXPECT_EQ(pDs->update(), false);
}

TEST_F(DistanceSensor_test,
       update_smallValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pSonarMock, ping_cm(0)).WillOnce(Return(9));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_EQ(pDs->update(), true);
}

TEST_F(DistanceSensor_test,
       update_largeValueDiff_smallTimeDiff_shall_return_true) {
  EXPECT_CALL(*pSonarMock, ping_cm(0)).WillOnce(Return(10));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_EQ(pDs->update(), true);
}

TEST_F(DistanceSensor_test,
       update_largeValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pSonarMock, ping_cm(0)).WillOnce(Return(10));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_EQ(pDs->update(), true);
}

// TODO: Add test of update where configs are 0

TEST_F(DistanceSensor_test, getDiscoveryMsg) {
  uint8_t buf[15] = {};
  EXPECT_EQ(pDs->getDiscoveryMsg(buf), 15);
  EXPECT_THAT(
      buf,
      ElementsAre(
          7, static_cast<uint8_t>(Component::Type::sensor),
          static_cast<uint8_t>(SensorDeviceClass::distance),
          static_cast<uint8_t>(Unit::Type::cm), (sizeof(DistanceT) << 4) | 0, 3,
          0, static_cast<uint8_t>(Unit::Type::cm), (sizeof(DistanceT) << 4) | 0,
          1, static_cast<uint8_t>(Unit::Type::s), (sizeof(uint16_t) << 4) | 0,
          2, static_cast<uint8_t>(Unit::Type::s), (sizeof(uint16_t) << 4) | 0));
}

TEST_F(DistanceSensor_test, getConfigItemValuesMsg) {
  uint8_t buf[11] = {};
  EXPECT_EQ(pDs->getConfigItemValuesMsg(buf), 11);
  EXPECT_THAT(buf, ElementsAre(7, 3, 0, 0, 10, 1, 0, 60, 2, 0, 60));
}

TEST_F(DistanceSensor_test, setConfigs) {
  uint8_t buf[] = {0, 0x03, 0xE8, 1, 0x03, 0xE9, 2, 0x03, 0xEA};
  EXPECT_TRUE(pDs->setConfigs(3, buf));
  uint8_t expect_buf[11] = {};
  EXPECT_EQ(pDs->getConfigItemValuesMsg(expect_buf), 11);
  EXPECT_THAT(expect_buf, ElementsAre(7, 3, 0, 0x03, 0xE8,  // 1000
                                      1, 0x03, 0xE9,        // 1001
                                      2, 0x03, 0xEA));      // 1002
}

TEST_F(DistanceSensor_test, setConfigs_wrong_number) {
  uint8_t buf[] = {0, 0x03, 0xE8, 1, 0x03, 0xE9, 2, 0x03, 0xEA};
  EXPECT_FALSE(pDs->setConfigs(2, buf));
}

TEST_F(DistanceSensor_test,
       update_largeValueDiff_largeTimeDiff_withConfigsZero_shall_return_false) {
  EXPECT_CALL(*pSonarMock, ping_cm(0)).WillOnce(Return(10));
  EXPECT_CALL(*pArduinoMock, millis()).Times(0);
  uint8_t buf[] = {0, 0, 0, 1, 0, 0, 2, 0, 0};
  EXPECT_EQ(pDs->setConfigs(3, buf), true);
  EXPECT_EQ(pDs->update(), false);
}
