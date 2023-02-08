#include "Sensor.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/Arduino.h"

using ::testing::ElementsAre;
using ::testing::Return;

class SensorInt8_test : public ::testing::Test {
 protected:
  Sensor<int8_t> sc = Sensor<int8_t>(108, "Sensor8");
};

class SensorInt16_test : public ::testing::Test {
 protected:
  Sensor<int16_t> sc = Sensor<int16_t>(116, "Sensor16");
};

class SensorInt32_test : public ::testing::Test {
 protected:
  Sensor<int32_t> sc = Sensor<int32_t>(132, "Sensor32");
};

class SensorInt8BatteryCm_test : public ::testing::Test {
 protected:
  Sensor<int8_t> sc = Sensor<int8_t>(108, "Sensor8", SensorDeviceClass::battery,
                                     Unit::Type::cm);
};

class SensorInt16BatteryCm_test : public ::testing::Test {
 protected:
  Sensor<int16_t> sc = Sensor<int16_t>(
      116, "Sensor16", SensorDeviceClass::battery, Unit::Type::cm);
};

class SensorInt32BatteryCm_test : public ::testing::Test {
 protected:
  Sensor<int32_t> sc = Sensor<int32_t>(
      132, "Sensor32", SensorDeviceClass::battery, Unit::Type::cm);
};

TEST_F(SensorInt8_test, hasService_no) { EXPECT_EQ(sc.hasService(), false); }

TEST_F(SensorInt16_test, hasService_no) { EXPECT_EQ(sc.hasService(), false); }

TEST_F(SensorInt32_test, hasService_no) { EXPECT_EQ(sc.hasService(), false); }

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

TEST_F(SensorInt16_test, getComponentType) {
  EXPECT_EQ(sc.getComponentType(), Component::Type::sensor);
}

TEST_F(SensorInt32_test, getComponentType) {
  EXPECT_EQ(sc.getComponentType(), Component::Type::sensor);
}

TEST_F(SensorInt8_test, getDeviceClass) {
  EXPECT_EQ(sc.getDeviceClass(), SensorDeviceClass::none);
}

TEST_F(SensorInt16_test, getDeviceClass) {
  EXPECT_EQ(sc.getDeviceClass(), SensorDeviceClass::none);
}

TEST_F(SensorInt32_test, getDeviceClass) {
  EXPECT_EQ(sc.getDeviceClass(), SensorDeviceClass::none);
}

TEST_F(SensorInt8_test, getUnitType_default) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::none);
}

TEST_F(SensorInt16_test, getUnitType_default) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::none);
}

TEST_F(SensorInt32_test, getUnitType_default) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::none);
}

TEST_F(SensorInt8BatteryCm_test, getUnitType_cm) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::cm);
}

TEST_F(SensorInt16BatteryCm_test, getUnitType_cm) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::cm);
}

TEST_F(SensorInt32BatteryCm_test, getUnitType_cm) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::cm);
}

TEST_F(SensorInt8_test, getUnitName_default) {
  EXPECT_STREQ(sc.getUnitName(), "");
}

TEST_F(SensorInt16_test, getUnitName_default) {
  EXPECT_STREQ(sc.getUnitName(), "");
}

TEST_F(SensorInt32_test, getUnitName_default) {
  EXPECT_STREQ(sc.getUnitName(), "");
}

TEST_F(SensorInt8BatteryCm_test, getUnitName_cm) {
  EXPECT_STREQ(sc.getUnitName(), "cm");
}

TEST_F(SensorInt16BatteryCm_test, getUnitName_cm) {
  EXPECT_STREQ(sc.getUnitName(), "cm");
}

TEST_F(SensorInt32BatteryCm_test, getUnitName_cm) {
  EXPECT_STREQ(sc.getUnitName(), "cm");
}

TEST_F(SensorInt8_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(sc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(buf,
              ElementsAre(108, static_cast<uint8_t>(Component::Type::sensor),
                          static_cast<uint8_t>(SensorDeviceClass::none),
                          static_cast<uint8_t>(Unit::Type::none),
                          (sizeof(int8_t) << 4) | 0));
}

TEST_F(SensorInt16_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(sc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(buf,
              ElementsAre(116, static_cast<uint8_t>(Component::Type::sensor),
                          static_cast<uint8_t>(SensorDeviceClass::none),
                          static_cast<uint8_t>(Unit::Type::none),
                          (sizeof(int16_t) << 4) | 0));
}

TEST_F(SensorInt32_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(sc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(buf,
              ElementsAre(132, static_cast<uint8_t>(Component::Type::sensor),
                          static_cast<uint8_t>(SensorDeviceClass::none),
                          static_cast<uint8_t>(Unit::Type::none),
                          (sizeof(int32_t) << 4) | 0));
}

TEST_F(SensorInt32_test, getDiscoveryMsg_precision0) {
  Sensor<int32_t> sc2 = Sensor<int32_t>(
      132, "Sensor32", SensorDeviceClass::battery, Unit::Type::none, 0);
  uint8_t buf[5] = {};
  EXPECT_EQ(sc2.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(buf,
              ElementsAre(132, static_cast<uint8_t>(Component::Type::sensor),
                          static_cast<uint8_t>(SensorDeviceClass::battery),
                          static_cast<uint8_t>(Unit::Type::none),
                          (sizeof(int32_t) << 4) | 0));
}

TEST_F(SensorInt32_test, getDiscoveryMsg_precision1) {
  Sensor<int32_t> sc2 = Sensor<int32_t>(
      132, "Sensor32", SensorDeviceClass::battery, Unit::Type::none, 1);
  uint8_t buf[5] = {};
  EXPECT_EQ(sc2.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(buf,
              ElementsAre(132, static_cast<uint8_t>(Component::Type::sensor),
                          static_cast<uint8_t>(SensorDeviceClass::battery),
                          static_cast<uint8_t>(Unit::Type::none),
                          (sizeof(int32_t) << 4) | 1));
}

TEST_F(SensorInt32_test, getDiscoveryMsg_precision2) {
  Sensor<int32_t> sc2 = Sensor<int32_t>(
      132, "Sensor32", SensorDeviceClass::battery, Unit::Type::none, 2);
  uint8_t buf[5] = {};
  EXPECT_EQ(sc2.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(buf,
              ElementsAre(132, static_cast<uint8_t>(Component::Type::sensor),
                          static_cast<uint8_t>(SensorDeviceClass::battery),
                          static_cast<uint8_t>(Unit::Type::none),
                          (sizeof(int32_t) << 4) | 2));
}

TEST_F(SensorInt32_test, getDiscoveryMsg_precision3) {
  Sensor<int32_t> sc2 = Sensor<int32_t>(
      132, "Sensor32", SensorDeviceClass::battery, Unit::Type::none, 3);
  uint8_t buf[5] = {};
  EXPECT_EQ(sc2.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(buf,
              ElementsAre(132, static_cast<uint8_t>(Component::Type::sensor),
                          static_cast<uint8_t>(SensorDeviceClass::battery),
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
