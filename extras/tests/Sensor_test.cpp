#include "Sensor.h"

#include <gtest/gtest.h>

#include "Types.h"
#include "Unit.h"
#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"

using ::testing::Return;

class SensorPrint_test : public ::testing::Test {
 protected:
  void SetUp() override { strBuf[0] = '\0'; }

  void TearDown() override {}

  void bufSerReadStr() {
    size_t i = 0;
    while (Serial.available()) {
      int c = Serial.read();
      if (c < 0) {
        break;
      }
      strBuf[i++] = static_cast<char>(c);
    }
    strBuf[i] = '\0';
  }

  char strBuf[256];
};

class SensorInt8_test : public ::testing::Test {
 protected:
  Sensor<int8_t> sc = Sensor<int8_t>(108, "Sensor8");
};

class SensorUInt16_test : public ::testing::Test {
 protected:
  Sensor<uint16_t> sc = Sensor<uint16_t>(116, "Sensor16");
};

class SensorInt32_test : public ::testing::Test {
 protected:
  Sensor<int32_t> sc = Sensor<int32_t>(132, "Sensor32");
};

class SensorInt8BatteryPercent_test : public ::testing::Test {
 protected:
  Sensor<int8_t> sc = Sensor<int8_t>(108, "Sensor8", SensorDeviceClass::battery,
                                     Unit::Type::percent);
};

class SensorUInt16BatteryPercent_test : public ::testing::Test {
 protected:
  Sensor<uint16_t> sc = Sensor<uint16_t>(
      116, "Sensor16", SensorDeviceClass::battery, Unit::Type::percent);
};

class SensorInt32BatteryPercent_test : public ::testing::Test {
 protected:
  Sensor<int32_t> sc = Sensor<int32_t>(
      132, "Sensor32", SensorDeviceClass::battery, Unit::Type::percent);
};

TEST_F(SensorInt8_test, getValue) {
  EXPECT_EQ(sc.getValue(), 0);
  sc.setValue(INT8_MIN);
  EXPECT_EQ(sc.getValue(), INT8_MIN);
}

TEST_F(SensorUInt16_test, getValue) {
  EXPECT_EQ(sc.getValue(), 0);
  sc.setValue(1);
  EXPECT_EQ(sc.getValue(), 1);
}

TEST_F(SensorInt32_test, getValue) {
  EXPECT_EQ(sc.getValue(), 0);
  sc.setValue(INT32_MIN);
  EXPECT_EQ(sc.getValue(), INT32_MIN);
}

TEST_F(SensorInt8_test, getComponentType) {
  EXPECT_EQ(sc.getComponentType(), BaseComponent::Type::sensor);
}

TEST_F(SensorUInt16_test, getComponentType) {
  EXPECT_EQ(sc.getComponentType(), BaseComponent::Type::sensor);
}

TEST_F(SensorInt32_test, getComponentType) {
  EXPECT_EQ(sc.getComponentType(), BaseComponent::Type::sensor);
}

TEST_F(SensorInt8_test, getDeviceClass) {
  EXPECT_EQ(sc.getDeviceClass(), SensorDeviceClass::none);
}

TEST_F(SensorUInt16_test, getDeviceClass) {
  EXPECT_EQ(sc.getDeviceClass(), SensorDeviceClass::none);
}

TEST_F(SensorInt32_test, getDeviceClass) {
  EXPECT_EQ(sc.getDeviceClass(), SensorDeviceClass::none);
}

TEST_F(SensorInt8_test, getUnitType_default) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::none);
}

TEST_F(SensorUInt16_test, getUnitType_default) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::none);
}

TEST_F(SensorInt32_test, getUnitType_default) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::none);
}

TEST_F(SensorInt8BatteryPercent_test, getUnitType_percent) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::percent);
}

TEST_F(SensorUInt16BatteryPercent_test, getUnitType_percent) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::percent);
}

TEST_F(SensorInt32BatteryPercent_test, getUnitType_percent) {
  EXPECT_EQ(sc.getUnitType(), Unit::Type::percent);
}

TEST_F(SensorInt8_test, getDiscoveryEntityItem) {
  DiscoveryEntityItemT item;

  sc.getDiscoveryEntityItem(&item);

  EXPECT_EQ(item.entityId, 108);
  EXPECT_EQ(item.componentType,
            static_cast<uint8_t>(BaseComponent::Type::sensor));
  EXPECT_EQ(item.deviceClass, static_cast<uint8_t>(SensorDeviceClass::none));
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_TRUE(item.isSigned);
  EXPECT_EQ(item.sizeCode, 0);
  EXPECT_EQ(item.precision, 0);
}

TEST_F(SensorUInt16_test, getDiscoveryEntityItem) {
  DiscoveryEntityItemT item;

  sc.getDiscoveryEntityItem(&item);

  EXPECT_EQ(item.entityId, 116);
  EXPECT_EQ(item.componentType,
            static_cast<uint8_t>(BaseComponent::Type::sensor));
  EXPECT_EQ(item.deviceClass, static_cast<uint8_t>(SensorDeviceClass::none));
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_FALSE(item.isSigned);
  EXPECT_EQ(item.sizeCode, 1);
  EXPECT_EQ(item.precision, 0);
}

TEST_F(SensorInt32_test, getDiscoveryEntityItem) {
  DiscoveryEntityItemT item;

  sc.getDiscoveryEntityItem(&item);

  EXPECT_EQ(item.entityId, 132);
  EXPECT_EQ(item.componentType,
            static_cast<uint8_t>(BaseComponent::Type::sensor));
  EXPECT_EQ(item.deviceClass, static_cast<uint8_t>(SensorDeviceClass::none));
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_TRUE(item.isSigned);
  EXPECT_EQ(item.sizeCode, 2);
  EXPECT_EQ(item.precision, 0);
}

TEST_F(SensorInt32_test, getDiscoveryEntityItem_precision0) {
  Sensor<int32_t> sc2 = Sensor<int32_t>(
      132, "Sensor32", SensorDeviceClass::battery, Unit::Type::none, 0);
  DiscoveryEntityItemT item;

  sc2.getDiscoveryEntityItem(&item);

  EXPECT_EQ(item.entityId, 132);
  EXPECT_EQ(item.componentType,
            static_cast<uint8_t>(BaseComponent::Type::sensor));
  EXPECT_EQ(item.deviceClass, static_cast<uint8_t>(SensorDeviceClass::battery));
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_TRUE(item.isSigned);
  EXPECT_EQ(item.sizeCode, 2);
  EXPECT_EQ(item.precision, 0);
}

TEST_F(SensorInt32_test, getDiscoveryEntityItem_precision1) {
  Sensor<int32_t> sc2 = Sensor<int32_t>(
      132, "Sensor32", SensorDeviceClass::battery, Unit::Type::none, 1);
  DiscoveryEntityItemT item;

  sc2.getDiscoveryEntityItem(&item);

  EXPECT_EQ(item.entityId, 132);
  EXPECT_EQ(item.componentType,
            static_cast<uint8_t>(BaseComponent::Type::sensor));
  EXPECT_EQ(item.deviceClass, static_cast<uint8_t>(SensorDeviceClass::battery));
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_TRUE(item.isSigned);
  EXPECT_EQ(item.sizeCode, 2);
  EXPECT_EQ(item.precision, 1);
}

TEST_F(SensorInt32_test, getDiscoveryEntityItem_precision2) {
  Sensor<int32_t> sc2 = Sensor<int32_t>(
      132, "Sensor32", SensorDeviceClass::battery, Unit::Type::none, 2);
  DiscoveryEntityItemT item;

  sc2.getDiscoveryEntityItem(&item);

  EXPECT_EQ(item.entityId, 132);
  EXPECT_EQ(item.componentType,
            static_cast<uint8_t>(BaseComponent::Type::sensor));
  EXPECT_EQ(item.deviceClass, static_cast<uint8_t>(SensorDeviceClass::battery));
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_TRUE(item.isSigned);
  EXPECT_EQ(item.sizeCode, 2);
  EXPECT_EQ(item.precision, 2);
}

TEST_F(SensorInt32_test, getDiscoveryEntityItem_precision3) {
  Sensor<int32_t> sc2 = Sensor<int32_t>(
      132, "Sensor32", SensorDeviceClass::battery, Unit::Type::none, 3);
  DiscoveryEntityItemT item;

  sc2.getDiscoveryEntityItem(&item);

  EXPECT_EQ(item.entityId, 132);
  EXPECT_EQ(item.componentType,
            static_cast<uint8_t>(BaseComponent::Type::sensor));
  EXPECT_EQ(item.deviceClass, static_cast<uint8_t>(SensorDeviceClass::battery));
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_TRUE(item.isSigned);
  EXPECT_EQ(item.sizeCode, 2);
  EXPECT_EQ(item.precision, 3);
}

TEST_F(SensorInt8_test, getValueItem) {
  ValueItemT item;

  sc.getValueItem(&item);

  EXPECT_EQ(item.entityId, 108);
  EXPECT_EQ(item.value, 0);

  sc.setValue(-15);

  sc.getValueItem(&item);

  EXPECT_EQ(item.entityId, 108);
  EXPECT_EQ(item.value, uint32_t(-15));
}

TEST_F(SensorUInt16_test, getValueItem) {
  sc.setValue(1555);
  ValueItemT item;

  sc.getValueItem(&item);

  EXPECT_EQ(item.entityId, 116);
  EXPECT_EQ(item.value, 1555);
}

TEST_F(SensorInt32_test, getValueItem) {
  sc.setValue(-150000000l);
  ValueItemT item;

  sc.getValueItem(&item);

  EXPECT_EQ(item.entityId, 132);
  EXPECT_EQ(item.value, uint32_t(-150000000l));
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

TEST_F(SensorPrint_test, print_simple_constructed_sensor) {
  const char* expectStr = ": 0";
  Sensor<int8_t> sc = Sensor<int8_t>(108);

  size_t printedChars = sc.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(SensorPrint_test, print_negative_value_scalefactor_1_unit_none) {
  const char* expectStr = "Sensor8: -123";
  Sensor<int8_t> sc = Sensor<int8_t>(108, "Sensor8");
  sc.setValue(-123);

  size_t printedChars = sc.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(SensorPrint_test, print_positive_value_scalefactor_1000_unit_percent) {
  const char* expectStr = "Sensor8: 0.001 %";
  Sensor<int8_t> sc = Sensor<int8_t>(
      108, "Sensor8", SensorDeviceClass::humidity, Unit::Type::percent, 3);
  sc.setValue(1);

  size_t printedChars = sc.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(SensorPrint_test, print_negative_value_scalefactor_10_unit_mm) {
  const char* expectStr = "Sensor16: 1234.5 mm";
  Sensor<uint16_t> sc = Sensor<uint16_t>(
      116, "Sensor16", SensorDeviceClass::distance, Unit::Type::mm, 1);
  sc.setValue(12345);

  size_t printedChars = sc.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(SensorPrint_test, print_negative_value_scalefactor_1000_unit_um) {
  const char* expectStr = "Sensor32: -2147483.648 μm";
  Sensor<int32_t> sc = Sensor<int32_t>(
      116, "Sensor32", SensorDeviceClass::distance, Unit::Type::um, 3);
  sc.setValue(INT32_MIN);

  size_t printedChars = sc.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(SensorInt8_test, isReportDue) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis()).WillOnce(Return(0));

  // Newly constructed shall be true
  EXPECT_TRUE(sc.isReportDue());

  // Shall be set to false when setReported() is called
  sc.setReported();
  EXPECT_FALSE(sc.isReportDue());

  // Shall set it with setIsReportDue()
  sc.setIsReportDue(true);
  EXPECT_TRUE(sc.isReportDue());
  sc.setIsReportDue(false);
  EXPECT_FALSE(sc.isReportDue());

  releaseArduinoMock();
}
