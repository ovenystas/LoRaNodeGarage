#include "HeightSensor.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/DistanceSensor.h"

using ::testing::ElementsAre;
using ::testing::Return;

using HeightT = int16_t;  // cm

class HeightSensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    pDistanceSensorMock = new DistanceSensorMock();
    pHs = new HeightSensor(39, "HeightSensor", *pDistanceSensorMock);
  }

  void TearDown() override {
    delete pHs;
    delete pDistanceSensorMock;
    releaseArduinoMock();
  }

  ArduinoMock* pArduinoMock;
  DistanceSensorMock* pDistanceSensorMock;
  HeightSensor* pHs;
};

TEST_F(HeightSensor_test,
       update_smallValueDiff_smallTimeDiff_shall_return_false) {
  EXPECT_CALL(*pDistanceSensorMock, getValue()).WillOnce(Return(69));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_FALSE(pHs->update());
}

TEST_F(HeightSensor_test,
       update_smallValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDistanceSensorMock, getValue()).WillOnce(Return(69));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pHs->update());
}

TEST_F(HeightSensor_test,
       update_largeValueDiff_smallTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDistanceSensorMock, getValue()).WillOnce(Return(70));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_TRUE(pHs->update());
}

TEST_F(HeightSensor_test,
       update_largeValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDistanceSensorMock, getValue()).WillOnce(Return(70));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pHs->update());
}

TEST_F(HeightSensor_test, getDiscoveryMsg) {
  uint8_t buf[18] = {};
  EXPECT_EQ(pHs->getDiscoveryMsg(buf), 18);
  // clang-format off
  EXPECT_THAT(
      buf,
      ElementsAre(
          39,
          static_cast<uint8_t>(Component::Type::sensor),
          static_cast<uint8_t>(SensorDeviceClass::distance),
          static_cast<uint8_t>(Unit::Type::cm),
          (sizeof(HeightT) << 4) | 0,
          4,
          0, static_cast<uint8_t>(Unit::Type::cm), (sizeof(HeightT) << 4) | 0,
          1, static_cast<uint8_t>(Unit::Type::s), (sizeof(uint16_t) << 4) | 0,
          2, static_cast<uint8_t>(Unit::Type::ms), (sizeof(uint16_t) << 4) | 0,
          3, static_cast<uint8_t>(Unit::Type::cm), (sizeof(HeightT) << 4) | 0
  ));
  // clang-format on
}

TEST_F(HeightSensor_test, getConfigItemValuesMsg) {
  uint8_t buf[14] = {};
  EXPECT_EQ(pHs->getConfigItemValuesMsg(buf), 14);
  // clang-format off
  EXPECT_THAT(
    buf,
    ElementsAre(
        39, 4,
        0, highByte(10), lowByte(10),
        1, highByte(60), lowByte(60),
        2, highByte(5000), lowByte(5000),
        3, highByte(60), lowByte(60)
  ));
  // clang-format on
}

TEST_F(HeightSensor_test, setConfigs_all_in_order) {
  uint8_t buf[] = {
      // clang-format off
      0, highByte(1000), lowByte(1000),
      1, highByte(1001), lowByte(1001),
      2, highByte(1002), lowByte(1002),
      3, highByte(1003), lowByte(1003)
      // clang-format on
  };
  EXPECT_TRUE(pHs->setConfigs(4, buf));
  uint8_t expect_buf[14] = {};
  EXPECT_EQ(pHs->getConfigItemValuesMsg(expect_buf), 14);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
        39, 4,
        0, highByte(1000), lowByte(1000),
        1, highByte(1001), lowByte(1001),
        2, highByte(1002), lowByte(1002),
        3, highByte(1003), lowByte(1003)
  ));
  // clang-format on
}

TEST_F(HeightSensor_test, setConfigs_all_out_of_order) {
  uint8_t buf[] = {
      // clang-format off
      3, highByte(2003), lowByte(2003),
      2, highByte(2002), lowByte(2002),
      1, highByte(2001), lowByte(2001),
      0, highByte(2000), lowByte(2000)
      // clang-format on
  };
  EXPECT_TRUE(pHs->setConfigs(4, buf));
  uint8_t expect_buf[14] = {};
  EXPECT_EQ(pHs->getConfigItemValuesMsg(expect_buf), 14);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
      39, 4,
      0, highByte(2000), lowByte(2000),
      1, highByte(2001), lowByte(2001),
      2, highByte(2002), lowByte(2002),
      3, highByte(2003), lowByte(2003)
  ));
  // clang-format on
}

TEST_F(HeightSensor_test, setConfigs_one) {
  uint8_t buf[] = {
      // clang-format off
      3, highByte(3003), lowByte(3003)
      // clang-format on
  };
  EXPECT_TRUE(pHs->setConfigs(1, buf));
  uint8_t expect_buf[14] = {};
  EXPECT_EQ(pHs->getConfigItemValuesMsg(expect_buf), 14);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
        39, 4,
        0, highByte(10), lowByte(10),
        1, highByte(60), lowByte(60),
        2, highByte(5000), lowByte(5000),
        3, highByte(3003), lowByte(3003)
  ));
  // clang-format on
}

TEST_F(HeightSensor_test, setConfigs_too_many) {
  uint8_t buf[] = {
      // clang-format off
      0, highByte(1000), lowByte(1000),
      1, highByte(1001), lowByte(1001),
      2, highByte(1002), lowByte(1002),
      3, highByte(1003), lowByte(1003),
      4, highByte(1004), lowByte(1004)
      // clang-format on
  };
  EXPECT_FALSE(pHs->setConfigs(5, buf));
}

TEST_F(HeightSensor_test, setConfigs_out_of_range) {
  uint8_t buf[] = {
      // clang-format off
      4, highByte(3004), lowByte(3004)
      // clang-format on
  };
  EXPECT_FALSE(pHs->setConfigs(1, buf));
}

TEST_F(HeightSensor_test,
       update_largeValueDiff_largeTimeDiff_withConfigsZero_shall_return_false) {
  EXPECT_CALL(*pDistanceSensorMock, getValue()).WillOnce(Return(70));
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
