#include "TemperatureSensor.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/DHT.h"

using ::testing::ElementsAre;
using ::testing::Return;

using TemperatureT = int16_t;  // Degree C

class TemperatureSensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    pDhtMock = new DHTMock();
    pTs = new TemperatureSensor(15, "TemperatureSensor", *pDhtMock);
  }

  void TearDown() override {
    delete pTs;
    delete pDhtMock;
    releaseArduinoMock();
  }

  ArduinoMock *pArduinoMock;
  DHTMock *pDhtMock;
  TemperatureSensor *pTs;
};

TEST_F(TemperatureSensor_test,
       update_smallValueDiff_smallTimeDiff_shall_return_false) {
  EXPECT_CALL(*pDhtMock, readTemperature(false, false)).WillOnce(Return(0.949));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_FALSE(pTs->update());
}

TEST_F(TemperatureSensor_test,
       update_smallValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDhtMock, readTemperature(false, false)).WillOnce(Return(0.949));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pTs->update());
}

TEST_F(TemperatureSensor_test,
       update_largeValueDiff_smallTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDhtMock, readTemperature(false, false)).WillOnce(Return(0.950));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_TRUE(pTs->update());
}

TEST_F(TemperatureSensor_test,
       update_largeValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDhtMock, readTemperature(false, false)).WillOnce(Return(0.950));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pTs->update());
}

TEST_F(TemperatureSensor_test, getDiscoveryMsg) {
  uint8_t buf[18] = {};
  EXPECT_EQ(pTs->getDiscoveryMsg(buf), 18);
  // clang-format off
  EXPECT_THAT(
      buf,
      ElementsAre(
          15,
          static_cast<uint8_t>(BaseComponent::Type::sensor),
          static_cast<uint8_t>(SensorDeviceClass::temperature),
          static_cast<uint8_t>(Unit::Type::C),
          (sizeof(TemperatureT) << 4) | 1,
          4,
          0, static_cast<uint8_t>(Unit::Type::C), (sizeof(TemperatureT) << 4) | 1,
          1, static_cast<uint8_t>(Unit::Type::s), (sizeof(uint16_t) << 4) | 0,
          2, static_cast<uint8_t>(Unit::Type::s), (sizeof(uint16_t) << 4) | 0,
          3, static_cast<uint8_t>(Unit::Type::C), (sizeof(TemperatureT) << 4) | 0
  ));
  // clang-format on
}

TEST_F(TemperatureSensor_test, getConfigItemValuesMsg) {
  uint8_t buf[14] = {};
  EXPECT_EQ(pTs->getConfigItemValuesMsg(buf), 14);
  // clang-format off
  EXPECT_THAT(
    buf,
    ElementsAre(
        15, 4,
        0, highByte(10), lowByte(10),
        1, highByte(60), lowByte(60),
        2, highByte(60), lowByte(60),
        3, highByte(0), lowByte(0)
  ));
  // clang-format on
}

TEST_F(TemperatureSensor_test, setConfigs_all_in_order) {
  uint8_t buf[] = {
      // clang-format off
      0, highByte(1000), lowByte(1000),
      1, highByte(1001), lowByte(1001),
      2, highByte(1002), lowByte(1002),
      3, highByte(1003), lowByte(1003)
      // clang-format on
  };
  EXPECT_TRUE(pTs->setConfigs(4, buf));
  uint8_t expect_buf[14] = {};
  EXPECT_EQ(pTs->getConfigItemValuesMsg(expect_buf), 14);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
        15, 4,
        0, highByte(1000), lowByte(1000),
        1, highByte(1001), lowByte(1001),
        2, highByte(1002), lowByte(1002),
        3, highByte(1003), lowByte(1003)
  ));
  // clang-format on
}

TEST_F(TemperatureSensor_test, setConfigs_all_out_of_order) {
  uint8_t buf[] = {
      // clang-format off
      3, highByte(2003), lowByte(2003),
      2, highByte(2002), lowByte(2002),
      1, highByte(2001), lowByte(2001),
      0, highByte(2000), lowByte(2000)
      // clang-format on
  };
  EXPECT_TRUE(pTs->setConfigs(4, buf));
  uint8_t expect_buf[14] = {};
  EXPECT_EQ(pTs->getConfigItemValuesMsg(expect_buf), 14);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
      15, 4,
      0, highByte(2000), lowByte(2000),
      1, highByte(2001), lowByte(2001),
      2, highByte(2002), lowByte(2002),
      3, highByte(2003), lowByte(2003)
  ));
  // clang-format on
}

TEST_F(TemperatureSensor_test, setConfigs_one) {
  uint8_t buf[] = {
      // clang-format off
      3, highByte(3003), lowByte(3003)
      // clang-format on
  };
  EXPECT_TRUE(pTs->setConfigs(1, buf));
  uint8_t expect_buf[14] = {};
  EXPECT_EQ(pTs->getConfigItemValuesMsg(expect_buf), 14);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
        15, 4,
        0, highByte(10), lowByte(10),
        1, highByte(60), lowByte(60),
        2, highByte(60), lowByte(60),
        3, highByte(3003), lowByte(3003)
  ));
  // clang-format on
}

TEST_F(TemperatureSensor_test, setConfigs_too_many) {
  uint8_t buf[] = {
      // clang-format off
      0, highByte(1000), lowByte(1000),
      1, highByte(1001), lowByte(1001),
      2, highByte(1002), lowByte(1002),
      3, highByte(1003), lowByte(1003),
      4, highByte(1004), lowByte(1004)
      // clang-format on
  };
  EXPECT_FALSE(pTs->setConfigs(5, buf));
}

TEST_F(TemperatureSensor_test, setConfigs_out_of_range) {
  uint8_t buf[] = {
      // clang-format off
      4, highByte(3004), lowByte(3004)
      // clang-format on
  };
  EXPECT_FALSE(pTs->setConfigs(1, buf));
}

TEST_F(TemperatureSensor_test,
       update_largeValueDiff_largeTimeDiff_withConfigsZero_shall_return_false) {
  EXPECT_CALL(*pDhtMock, readTemperature(false, false)).WillOnce(Return(0.950));
  EXPECT_CALL(*pArduinoMock, millis()).Times(0);
  uint8_t buf[] = {
      // clang-format off
      0, 0, 0,
      1, 0, 0,
      2, 0, 0,
      3, 0, 0
      // clang-format on
  };
  EXPECT_TRUE(pTs->setConfigs(4, buf));
  EXPECT_FALSE(pTs->update());
}
