#include "../Sensor.h"

#include <gtest/gtest.h>

#include "../Unit.h"
#include "Sensor_test.h"
#include "mocks/Arduino.h"

using ::testing::ElementsAre;
using ::testing::Return;

TEST_F(SensorInt8_test, hasService_no) { EXPECT_EQ(sc.hasService(), false); }

TEST_F(SensorInt16_test, hasService_no) { EXPECT_EQ(sc.hasService(), false); }

TEST_F(SensorInt32_test, hasService_no) { EXPECT_EQ(sc.hasService(), false); }

// TEST_F(SensorInt8_test, callService) {
//   sc.callService(static_cast<uint8_t>(0u));
//   SUCCEED();
// }

TEST_F(SensorInt8_test, getValue) {
  EXPECT_EQ(sc.getValue(), 0);
  sc.setValue(INT8_MIN);
  EXPECT_EQ(sc.getValue(), INT8_MIN);
}

TEST_F(SensorInt16_test, getValue) {
  EXPECT_EQ(sc.getValue(), 0);
  sc.setValue(INT16_MIN);
  EXPECT_EQ(sc.getValue(), INT16_MIN);
}

TEST_F(SensorInt32_test, getValue) {
  EXPECT_EQ(sc.getValue(), 0);
  sc.setValue(INT32_MIN);
  EXPECT_EQ(sc.getValue(), INT32_MIN);
}

TEST_F(SensorInt8_test, getComponentType) {
  EXPECT_EQ(sc.getComponentType(), Component::Type::sensor);
}

TEST_F(SensorInt8_test, getDeviceClass) {
  EXPECT_EQ(sc.getDeviceClass(), Sensor<int8_t>::DeviceClass::none);
}

TEST_F(SensorInt8_test, getUnitType_default) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::none);
}

TEST(Sensor_test, getUnitType_cm) {
  SensorChild<int8_t> sc = SensorChild<int8_t>(108, "Sensor8", Unit::Type::cm);
  EXPECT_EQ(sc.getUnitType(), Unit::Type::cm);
}

TEST_F(SensorInt8_test, getUnitName_default) {
  EXPECT_STREQ(sc.getUnitName(), "");
}

TEST(Sensor_test, getUnitName_cm) {
  SensorChild<int8_t> sc = SensorChild<int8_t>(108, "Sensor8", Unit::Type::cm);
  EXPECT_STREQ(sc.getUnitName(), "cm");
}

TEST_F(SensorInt8_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(sc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(108, static_cast<uint8_t>(Component::Type::sensor),
                       static_cast<uint8_t>(Sensor<int8_t>::DeviceClass::none),
                       static_cast<uint8_t>(Unit::Type::none),
                       (sizeof(int8_t) << 4) | 0));
}

TEST_F(SensorInt16_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(sc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(116, static_cast<uint8_t>(Component::Type::sensor),
                       static_cast<uint8_t>(Sensor<int8_t>::DeviceClass::none),
                       static_cast<uint8_t>(Unit::Type::none),
                       (sizeof(int16_t) << 4) | 0));
}

TEST_F(SensorInt32_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(sc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(132, static_cast<uint8_t>(Component::Type::sensor),
                       static_cast<uint8_t>(Sensor<int8_t>::DeviceClass::none),
                       static_cast<uint8_t>(Unit::Type::none),
                       (sizeof(int32_t) << 4) | 0));
}

TEST(Sensor_test, getDiscoveryMsg_precision0) {
  SensorChild<int32_t> sc =
      SensorChild<int32_t>(132, "Sensor32", Unit::Type::none, 0);
  uint8_t buf[5] = {};
  EXPECT_EQ(sc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(132, static_cast<uint8_t>(Component::Type::sensor),
                       static_cast<uint8_t>(Sensor<int8_t>::DeviceClass::none),
                       static_cast<uint8_t>(Unit::Type::none),
                       (sizeof(int32_t) << 4) | 0));
}

TEST(Sensor_test, getDiscoveryMsg_precision1) {
  SensorChild<int32_t> sc =
      SensorChild<int32_t>(132, "Sensor32", Unit::Type::none, 1);
  uint8_t buf[5] = {};
  EXPECT_EQ(sc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(132, static_cast<uint8_t>(Component::Type::sensor),
                       static_cast<uint8_t>(Sensor<int8_t>::DeviceClass::none),
                       static_cast<uint8_t>(Unit::Type::none),
                       (sizeof(int32_t) << 4) | 1));
}

TEST(Sensor_test, getDiscoveryMsg_precision2) {
  SensorChild<int32_t> sc =
      SensorChild<int32_t>(132, "Sensor32", Unit::Type::none, 2);
  uint8_t buf[5] = {};
  EXPECT_EQ(sc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(132, static_cast<uint8_t>(Component::Type::sensor),
                       static_cast<uint8_t>(Sensor<int8_t>::DeviceClass::none),
                       static_cast<uint8_t>(Unit::Type::none),
                       (sizeof(int32_t) << 4) | 2));
}

TEST(Sensor_test, getDiscoveryMsg_precision3) {
  SensorChild<int32_t> sc =
      SensorChild<int32_t>(132, "Sensor32", Unit::Type::none, 3);
  uint8_t buf[5] = {};
  EXPECT_EQ(sc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(132, static_cast<uint8_t>(Component::Type::sensor),
                       static_cast<uint8_t>(Sensor<int8_t>::DeviceClass::none),
                       static_cast<uint8_t>(Unit::Type::none),
                       (sizeof(int32_t) << 4) | 3));
}

TEST_F(SensorInt8_test, getValueMsg) {
  uint8_t buf[2] = {};
  EXPECT_EQ(sc.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(108, 0));
  sc.setValue(-15);
  EXPECT_EQ(sc.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(108, -15));
}

TEST_F(SensorInt16_test, getValueMsg) {
  uint8_t buf[3] = {};
  sc.setValue(-1555);
  EXPECT_EQ(sc.getValueMsg(buf), 3);
  EXPECT_THAT(buf, ElementsAre(116, 0xF9, 0xED));  // -1555=0xF9ED
}

TEST_F(SensorInt32_test, getValueMsg) {
  uint8_t buf[5] = {};
  sc.setValue(-150000000l);
  EXPECT_EQ(sc.getValueMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(132, 0xF7, 0x0F, 0x2E, 0x80));  // -150000000=0xF70F2E80
}

TEST_F(SensorInt8_test, setReported) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis())
      .WillOnce(Return(0))
      .WillOnce(Return(10000))
      .WillOnce(Return(20500))
      .WillOnce(Return(35999));
  sc.setReported();
  EXPECT_EQ(sc.absDiffLastReportedValue(), 0);
  EXPECT_EQ(sc.timeSinceLastReport(), 10);
  sc.setValue(-33);
  EXPECT_EQ(sc.absDiffLastReportedValue(), 33);
  sc.setReported();
  EXPECT_EQ(sc.timeSinceLastReport(), 15);
  releaseArduinoMock();
}

// TEST_F(SensorInt8_test, print) {
//   sc.print(Serial);
//   SUCCEED();
// }

// TEST_F(SensorInt8_test, print) {
//   sc.print(Serial, 0);
//   SUCCEED();
// }
