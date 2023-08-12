#include "BinarySensor.h"

#include <gtest/gtest.h>

#include "Types.h"
#include "Unit.h"
#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"

using ::testing::_;
using ::testing::Return;

class BinarySensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pSerial = new BufferSerial(256);
    pBS = new BinarySensor(34, "BinarySensor");
    strBuf[0] = '\0';
  }

  void TearDown() override {
    delete pBS;
    delete pSerial;
  }

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
  BinarySensor* pBS;
};

TEST(BinarySensor_getStateName_test, battery) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::battery);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "normal");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "low");
}

TEST(BinarySensor_getStateName_test, cold) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::cold);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "normal");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "cold");
}

TEST(BinarySensor_getStateName_test, heat) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::heat);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "normal");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "hot");
}

TEST(BinarySensor_getStateName_test, connectivity) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::connectivity);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "disconnected");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "connected");
}

TEST(BinarySensor_getStateName_test, door) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::door);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "closed");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "open");
}

TEST(BinarySensor_getStateName_test, garageDoor) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::garageDoor);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "closed");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "open");
}

TEST(BinarySensor_getStateName_test, opening) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::opening);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "closed");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "open");
}

TEST(BinarySensor_getStateName_test, window) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::window);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "closed");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "open");
}

TEST(BinarySensor_getStateName_test, lock) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::lock);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "locked");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "unlocked");
}

TEST(BinarySensor_getStateName_test, moisture) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::moisture);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "dry");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "wet");
}

TEST(BinarySensor_getStateName_test, gas) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::gas);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "clear");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "detected");
}

TEST(BinarySensor_getStateName_test, motion) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::motion);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "clear");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "detected");
}

TEST(BinarySensor_getStateName_test, occupancy) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::occupancy);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "clear");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "detected");
}

TEST(BinarySensor_getStateName_test, smoke) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::smoke);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "clear");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "detected");
}

TEST(BinarySensor_getStateName_test, sound) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::sound);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "clear");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "detected");
}

TEST(BinarySensor_getStateName_test, vibration) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::vibration);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "clear");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "detected");
}

TEST(BinarySensor_getStateName_test, presence) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::presence);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "away");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "home");
}

TEST(BinarySensor_getStateName_test, problem) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::problem);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "OK");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "problem");
}

TEST(BinarySensor_getStateName_test, safety) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::safety);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "safe");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "unsafe");
}

TEST(BinarySensor_getStateName_test, none) {
  BinarySensor bs =
      BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::none);
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "off");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "on");
}

TEST(BinarySensor_getStateName_test, unknown) {
  BinarySensor bs = BinarySensor(34, "BinarySensor",
                                 static_cast<BinarySensorDeviceClass>(255));
  bs.setState(false);
  EXPECT_STREQ(bs.getStateName(), "off");
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "on");
}

TEST_F(BinarySensor_test, getComponentType) {
  EXPECT_EQ(pBS->getComponentType(), BaseComponent::Type::binarySensor);
}

TEST_F(BinarySensor_test, getDeviceClass) {
  EXPECT_EQ(pBS->getDeviceClass(), BinarySensorDeviceClass::none);
}

TEST_F(BinarySensor_test, getDiscoveryEntityItem) {
  DiscoveryEntityItemT item;

  pBS->getDiscoveryEntityItem(&item);

  EXPECT_EQ(item.entityId, 34);
  EXPECT_EQ(item.componentType,
            static_cast<uint8_t>(BaseComponent::Type::binarySensor));
  EXPECT_EQ(item.deviceClass,
            static_cast<uint8_t>(BinarySensorDeviceClass::none));
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_FALSE(item.isSigned);
  EXPECT_EQ(item.sizeCode, 0);
  EXPECT_EQ(item.precision, 0);
}

TEST_F(BinarySensor_test, getEntityId) { EXPECT_EQ(pBS->getEntityId(), 34); }

TEST_F(BinarySensor_test, getState) {
  pBS->setState(false);
  EXPECT_EQ(pBS->getState(), false);

  pBS->setState(true);
  EXPECT_EQ(pBS->getState(), true);
}

TEST_F(BinarySensor_test, getValueItem) {
  ValueItemT item;
  pBS->setState(false);

  pBS->getValueItem(&item);

  EXPECT_EQ(item.entityId, 34);
  EXPECT_EQ(item.value, 0);

  pBS->setState(true);

  pBS->getValueItem(&item);

  EXPECT_EQ(item.entityId, 34);
  EXPECT_EQ(item.value, 1);
}

TEST_F(BinarySensor_test, print_deviceclass_none_state_false) {
  const char* expectStr = "BinarySensor: off";
  pBS->setState(false);

  size_t printedChars = pBS->print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(BinarySensor_test, print_deviceclass_none_state_true) {
  const char* expectStr = "BinarySensor: on";
  pBS->setState(true);

  size_t printedChars = pBS->print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(BinarySensor_test,
       setReported_isDiffLastReportedState_timeSinceLastReport) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis())
      .WillOnce(Return(0))
      .WillOnce(Return(10000))
      .WillOnce(Return(20500))
      .WillOnce(Return(35999));
  pBS->setReported();
  EXPECT_EQ(pBS->isDiffLastReportedState(), false);
  EXPECT_EQ(pBS->timeSinceLastReport(), 10);
  pBS->setState(true);
  EXPECT_EQ(pBS->isDiffLastReportedState(), true);
  pBS->setReported();
  EXPECT_EQ(pBS->timeSinceLastReport(), 15);

  releaseArduinoMock();
}

TEST_F(BinarySensor_test, isReportDue) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis()).WillOnce(Return(0));

  // Newly constructed shall be true
  EXPECT_TRUE(pBS->isReportDue());

  // Shall be set to false when setReported() is called
  pBS->setReported();
  EXPECT_FALSE(pBS->isReportDue());

  // Shall set it with setIsReportDue()
  pBS->setIsReportDue(true);
  EXPECT_TRUE(pBS->isReportDue());
  pBS->setIsReportDue(false);
  EXPECT_FALSE(pBS->isReportDue());

  releaseArduinoMock();
}
