#include "HeightSensor.h"

#include <gtest/gtest.h>

#include "Sensor.h"
#include "Unit.h"
#include "mocks/BufferSerial.h"

using ::testing::ElementsAre;
using ::testing::Return;

using HeightT = int16_t;  // cm

class HeightSensorPrint_test : public ::testing::Test {
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

class HeightSensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    pDistanceSensor = new Sensor<DistanceT>(
        99, "Distance", SensorDeviceClass::distance, Unit::Type::cm);
    pHs = new HeightSensor(39, "HeightSensor", *pDistanceSensor);
  }

  void TearDown() override {
    delete pHs;
    delete pDistanceSensor;
    releaseArduinoMock();
  }

  ArduinoMock* pArduinoMock;
  Sensor<DistanceT>* pDistanceSensor;
  HeightSensor* pHs;
};

TEST_F(HeightSensor_test, callService_shall_do_nothing) { pHs->callService(0); }

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

TEST_F(HeightSensor_test, getDiscoveryMsg) {
  uint8_t buf[18] = {};
  EXPECT_EQ(pHs->getDiscoveryMsg(buf), 18);
  // clang-format off
  EXPECT_THAT(
      buf,
      ElementsAre(
          39,
          static_cast<uint8_t>(BaseComponent::Type::sensor),
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

TEST_F(HeightSensor_test, getEntityId) { EXPECT_EQ(pHs->getEntityId(), 39); }

TEST_F(HeightSensor_test, getSensor) {
  Sensor<HeightT>& sensor = pHs->getSensor();
  EXPECT_EQ(sensor.getEntityId(), 39);
}

TEST_F(HeightSensor_test, getValueMsg) {
  uint8_t buf[3];
  EXPECT_EQ(pHs->getValueMsg(buf), 3);
  EXPECT_THAT(buf, ElementsAre(39, 0, 0));
}

TEST_F(HeightSensorPrint_test, print) {
  const char* expectStr = "HeightSensor: 0 cm";
  Sensor<DistanceT>* pDistanceSensor = new Sensor<DistanceT>(
      99, "Distance", SensorDeviceClass::distance, Unit::Type::cm);
  HeightSensor hs = HeightSensor(39, "HeightSensor", *pDistanceSensor);

  EXPECT_EQ(hs.print(*pSerial), 18);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
}

TEST_F(HeightSensorPrint_test, print_service_shall_do_nothing) {
  Sensor<DistanceT>* pDistanceSensor = new Sensor<DistanceT>(
      99, "Distance", SensorDeviceClass::distance, Unit::Type::cm);
  HeightSensor hs = HeightSensor(39, "HeightSensor", *pDistanceSensor);
  EXPECT_EQ(hs.print(*pSerial, 0), 0);
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

TEST_F(HeightSensor_test, setReported) {
  EXPECT_CALL(*pArduinoMock, millis()).Times(1);
  pHs->setReported();
}

TEST_F(HeightSensor_test,
       update_largeValueDiff_largeTimeDiff_withConfigsZero_shall_return_false) {
  pDistanceSensor->setValue(70);
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

TEST_F(HeightSensor_test,
       update_smallValueDiff_smallTimeDiff_shall_return_false) {
  pDistanceSensor->setValue(69);
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_FALSE(pHs->update());
}

TEST_F(HeightSensor_test,
       update_smallValueDiff_largeTimeDiff_shall_return_true) {
  pDistanceSensor->setValue(69);
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pHs->update());
}

TEST_F(HeightSensor_test,
       update_largeValueDiff_smallTimeDiff_shall_return_true) {
  pDistanceSensor->setValue(70);
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_TRUE(pHs->update());
}

TEST_F(HeightSensor_test,
       update_largeValueDiff_largeTimeDiff_shall_return_true) {
  pDistanceSensor->setValue(70);
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pHs->update());
}
