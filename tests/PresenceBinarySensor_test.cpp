#include "PresenceBinarySensor.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/Sensor.h"

using ::testing::ElementsAre;
using ::testing::Return;

using HeightT = int16_t;  // cm

class PresenceBinarySensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    pHeightSensorMock = new SensorMock<HeightT>();
    pPbs = new PresenceBinarySensor(89, "PresenceBinarySensor",
                                    *pHeightSensorMock);
  }

  void TearDown() override {
    delete pPbs;
    delete pHeightSensorMock;
    releaseArduinoMock();
  }

  ArduinoMock* pArduinoMock;
  SensorMock<HeightT>* pHeightSensorMock;
  PresenceBinarySensor* pPbs;
};

TEST_F(
    PresenceBinarySensor_test,
    update_belowLowLimit_belowMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  EXPECT_CALL(*pHeightSensorMock, getValue()).WillOnce(Return(179));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_aboveLowLimit_belowMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  EXPECT_CALL(*pHeightSensorMock, getValue()).WillOnce(Return(180));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowHighLimit_belowMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  EXPECT_CALL(*pHeightSensorMock, getValue()).WillOnce(Return(200));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_aboveHighLimit_belowMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  EXPECT_CALL(*pHeightSensorMock, getValue()).WillOnce(Return(201));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowLowLimit_aboveMinStableTime_belowLastReportedTimeLimit_shall_return_true) {
  EXPECT_CALL(*pHeightSensorMock, getValue()).WillOnce(Return(179));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(10000))
      .WillOnce(Return(59999));
  EXPECT_TRUE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_aboveLowLimit_aboveMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  EXPECT_CALL(*pHeightSensorMock, getValue()).WillOnce(Return(180));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(10000))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowHighLimit_aboveMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  EXPECT_CALL(*pHeightSensorMock, getValue()).WillOnce(Return(200));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(10000))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_aboveHighLimit_aboveMinStableTime_belowLastReportedTimeLimit_shall_return_true) {
  EXPECT_CALL(*pHeightSensorMock, getValue()).WillOnce(Return(201));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(10000))
      .WillOnce(Return(59999));
  EXPECT_TRUE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowLowLimit_belowMinStableTime_aboveLastReportedTimeLimit_shall_return_true) {
  EXPECT_CALL(*pHeightSensorMock, getValue()).WillOnce(Return(179));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(60000));
  EXPECT_TRUE(pPbs->update());
}

TEST_F(PresenceBinarySensor_test, getDiscoveryMsg) {
  uint8_t buf[18] = {};
  EXPECT_EQ(pPbs->getDiscoveryMsg(buf), 18);
  // clang-format off
  EXPECT_THAT(
      buf,
      ElementsAre(
          89,
          static_cast<uint8_t>(BaseComponent::Type::binarySensor),
          static_cast<uint8_t>(BinarySensorDeviceClass::presence),
          static_cast<uint8_t>(Unit::Type::none),
          (1 << 4) | 0,
          4,
          0, static_cast<uint8_t>(Unit::Type::cm), (sizeof(HeightT) << 4) | 0,
          1, static_cast<uint8_t>(Unit::Type::cm), (sizeof(HeightT) << 4) | 0,
          2, static_cast<uint8_t>(Unit::Type::ms), (sizeof(uint16_t) << 4) | 0,
          3, static_cast<uint8_t>(Unit::Type::s), (sizeof(uint16_t) << 4) | 0
  ));
  // clang-format on
}

TEST_F(PresenceBinarySensor_test, getConfigItemValuesMsg) {
  uint8_t buf[14] = {};
  EXPECT_EQ(pPbs->getConfigItemValuesMsg(buf), 14);
  // clang-format off
  EXPECT_THAT(
    buf,
    ElementsAre(
        89, 4,
        0, highByte(180), lowByte(180),
        1, highByte(200), lowByte(200),
        2, highByte(10000), lowByte(10000),
        3, highByte(60), lowByte(60)
  ));
  // clang-format on
}

TEST_F(PresenceBinarySensor_test, setConfigs_all_in_order) {
  uint8_t buf[] = {
      // clang-format off
      0, highByte(1000), lowByte(1000),
      1, highByte(1001), lowByte(1001),
      2, highByte(1002), lowByte(1002),
      3, highByte(1003), lowByte(1003)
      // clang-format on
  };
  EXPECT_TRUE(pPbs->setConfigs(4, buf));
  uint8_t expect_buf[14] = {};
  EXPECT_EQ(pPbs->getConfigItemValuesMsg(expect_buf), 14);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
        89, 4,
        0, highByte(1000), lowByte(1000),
        1, highByte(1001), lowByte(1001),
        2, highByte(1002), lowByte(1002),
        3, highByte(1003), lowByte(1003)
  ));
  // clang-format on
}

TEST_F(PresenceBinarySensor_test, setConfigs_all_out_of_order) {
  uint8_t buf[] = {
      // clang-format off
      3, highByte(2003), lowByte(2003),
      2, highByte(2002), lowByte(2002),
      1, highByte(2001), lowByte(2001),
      0, highByte(2000), lowByte(2000)
      // clang-format on
  };
  EXPECT_TRUE(pPbs->setConfigs(4, buf));
  uint8_t expect_buf[14] = {};
  EXPECT_EQ(pPbs->getConfigItemValuesMsg(expect_buf), 14);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
      89, 4,
      0, highByte(2000), lowByte(2000),
      1, highByte(2001), lowByte(2001),
      2, highByte(2002), lowByte(2002),
      3, highByte(2003), lowByte(2003)
  ));
  // clang-format on
}

TEST_F(PresenceBinarySensor_test, setConfigs_one) {
  uint8_t buf[] = {
      // clang-format off
      2, highByte(3002), lowByte(3002)
      // clang-format on
  };
  EXPECT_TRUE(pPbs->setConfigs(1, buf));
  uint8_t expect_buf[14] = {};
  EXPECT_EQ(pPbs->getConfigItemValuesMsg(expect_buf), 14);
  // clang-format off
  EXPECT_THAT(
    expect_buf,
    ElementsAre(
        89, 4,
        0, highByte(180), lowByte(180),
        1, highByte(200), lowByte(200),
        2, highByte(3002), lowByte(3002),
        3, highByte(60), lowByte(60)
));
  // clang-format on
}

TEST_F(PresenceBinarySensor_test, setConfigs_too_many) {
  uint8_t buf[] = {
      // clang-format off
      0, highByte(1000), lowByte(1000),
      1, highByte(1001), lowByte(1001),
      2, highByte(1002), lowByte(1002),
      3, highByte(1003), lowByte(1003),
      4, highByte(1004), lowByte(1004),
      // clang-format on
  };
  EXPECT_FALSE(pPbs->setConfigs(5, buf));
}

TEST_F(PresenceBinarySensor_test, setConfigs_out_of_range) {
  uint8_t buf[] = {
      // clang-format off
      4, highByte(3004), lowByte(3004)
      // clang-format on
  };
  EXPECT_FALSE(pPbs->setConfigs(1, buf));
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowLowLimit_belowMinStableTime_aboveLastReportedTimeLimit_zeroMinReportTime_shall_return_false) {
  EXPECT_CALL(*pHeightSensorMock, getValue()).WillOnce(Return(179));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(9999));
  uint8_t buf[] = {
      // clang-format off
      3, highByte(0), lowByte(0)
      // clang-format on
  };
  pPbs->setConfigs(1, buf);
  EXPECT_FALSE(pPbs->update());
}
