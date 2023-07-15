#include "PresenceBinarySensor.h"

#include <gtest/gtest.h>

#include "Sensor.h"
#include "Unit.h"
#include "mocks/BufferSerial.h"

using ::testing::ElementsAre;
using ::testing::Return;

using HeightT = int16_t;  // cm

class PresenceBinarySensorPrint_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pSerial = new BufferSerial(256);
    strBuf[0] = '\0';
  }

  void TearDown() override { delete pSerial; }

  void bufSerReadStr() {
    size_t i = 0;
    while (pSerial->available()) {
      int c = pSerial->read();
      if (c < 0) {
        break;
      }
      strBuf[i++] = static_cast<char>(c);
    }
    strBuf[i] = '\0';
  }

  char strBuf[256];
  BufferSerial* pSerial;
};

class PresenceBinarySensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    pHeightSensor = new Sensor<HeightT>(
        99, "Height", SensorDeviceClass::distance, Unit::Type::cm);
    pPbs = new PresenceBinarySensor(89, "PresenceBinarySensor", *pHeightSensor);
  }

  void TearDown() override {
    delete pPbs;
    delete pHeightSensor;
    releaseArduinoMock();
  }

  ArduinoMock* pArduinoMock;
  Sensor<HeightT>* pHeightSensor;
  PresenceBinarySensor* pPbs;
};

TEST_F(PresenceBinarySensor_test, callService_shall_do_nothing) {
  pPbs->callService(0);
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

TEST_F(PresenceBinarySensor_test, getEntityId) {
  EXPECT_EQ(pPbs->getEntityId(), 89);
}

TEST_F(PresenceBinarySensor_test, getValueMsg) {
  uint8_t buf[2];
  EXPECT_EQ(pPbs->getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(89, 0));
}

TEST_F(PresenceBinarySensorPrint_test, print) {
  const char* expectStr = "PresenceBinarySensor: away";
  Sensor<HeightT>* pHeightSensor = new Sensor<HeightT>(
      99, "Height", SensorDeviceClass::distance, Unit::Type::cm);
  PresenceBinarySensor pbs =
      PresenceBinarySensor(89, "PresenceBinarySensor", *pHeightSensor);

  EXPECT_EQ(pbs.print(*pSerial), strlen(expectStr));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
}

TEST_F(PresenceBinarySensorPrint_test, print_service_shall_do_nothing) {
  Sensor<HeightT>* pHeightSensor = new Sensor<HeightT>(
      99, "Height", SensorDeviceClass::distance, Unit::Type::cm);
  PresenceBinarySensor pbs =
      PresenceBinarySensor(89, "PresenceBinarySensor", *pHeightSensor);
  EXPECT_EQ(pbs.print(*pSerial, 0), 0);
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

TEST_F(PresenceBinarySensor_test, setReported) {
  EXPECT_CALL(*pArduinoMock, millis()).Times(1);
  pPbs->setReported();
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowLowLimit_belowMinStableTime_aboveLastReportedTimeLimit_zeroMinReportTime_shall_return_false) {
  pHeightSensor->setValue(179);
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(9999));
  uint8_t buf[] = {
      // clang-format off
      3, highByte(0), lowByte(0)
      // clang-format on
  };
  pPbs->setConfigs(1, buf);
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowLowLimit_belowMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  pHeightSensor->setValue(179);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_aboveLowLimit_belowMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  pHeightSensor->setValue(180);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowHighLimit_belowMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  pHeightSensor->setValue(200);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_aboveHighLimit_belowMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  pHeightSensor->setValue(201);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowLowLimit_aboveMinStableTime_belowLastReportedTimeLimit_shall_return_true) {
  pHeightSensor->setValue(179);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(10000))
      .WillOnce(Return(59999));
  EXPECT_TRUE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_aboveLowLimit_aboveMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  pHeightSensor->setValue(180);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(10000))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowHighLimit_aboveMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  pHeightSensor->setValue(200);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(10000))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_aboveHighLimit_aboveMinStableTime_belowLastReportedTimeLimit_shall_return_true) {
  pHeightSensor->setValue(201);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(10000))
      .WillOnce(Return(59999));
  EXPECT_TRUE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowLowLimit_belowMinStableTime_aboveLastReportedTimeLimit_shall_return_true) {
  pHeightSensor->setValue(179);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(60000));
  EXPECT_TRUE(pPbs->update());
}
