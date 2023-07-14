#include "BinarySensor.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"

using ::testing::_;
using ::testing::ElementsAre;
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

TEST_F(BinarySensor_test, getState_when_false) {
  EXPECT_EQ(pBS->getState(), false);
}

TEST_F(BinarySensor_test, getState_when_true) {
  pBS->setState(true);
  EXPECT_EQ(pBS->getState(), true);
}

TEST_F(BinarySensor_test, getStateName_when_false) {
  pBS->setState(false);
  EXPECT_STREQ(pBS->getStateName(), "off");
}

TEST_F(BinarySensor_test, getStateName_when_true) {
  pBS->setState(true);
  EXPECT_STREQ(pBS->getStateName(), "on");
}

TEST_F(BinarySensor_test, getComponentType) {
  EXPECT_EQ(pBS->getComponentType(), BaseComponent::Type::binarySensor);
}

TEST_F(BinarySensor_test, getDeviceClass) {
  EXPECT_EQ(pBS->getDeviceClass(), BinarySensorDeviceClass::none);
}

TEST_F(BinarySensor_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(pBS->getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf,
      ElementsAre(34, static_cast<uint8_t>(BaseComponent::Type::binarySensor),
                  static_cast<uint8_t>(BinarySensorDeviceClass::none),
                  static_cast<uint8_t>(Unit::Type::none), (1 << 4) | 0));
}

TEST_F(BinarySensor_test, getValueMsg_when_false) {
  uint8_t buf[2] = {};
  EXPECT_EQ(pBS->getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, false));
}

TEST_F(BinarySensor_test, getValueMsg_when_true) {
  uint8_t buf[2] = {};
  pBS->setState(true);
  EXPECT_EQ(pBS->getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, true));
}

TEST_F(BinarySensor_test, setReported) {
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

TEST_F(BinarySensor_test, print_deviceclass_battery_state_false) {
  const char* expectStr = "BinarySensor: normal";
  delete pBS;
  pBS = new BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::battery);
  pBS->setState(false);

  size_t printedChars = pBS->print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(BinarySensor_test, print_deviceclass_battery_state_true) {
  const char* expectStr = "BinarySensor: low";
  delete pBS;
  pBS = new BinarySensor(34, "BinarySensor", BinarySensorDeviceClass::battery);
  pBS->setState(true);

  size_t printedChars = pBS->print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}
