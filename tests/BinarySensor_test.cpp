#include "BinarySensor.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/Arduino.h"

using ::testing::ElementsAre;
using ::testing::Return;

class BinarySensor_test : public ::testing::Test {
 protected:
  BinarySensor bs = BinarySensor(34, "BinarySensor");
};

TEST_F(BinarySensor_test, hasService_no) { EXPECT_EQ(bs.hasService(), false); }

// TEST_F(BinarySensor_test, print) {
//   bs.print(Serial, 0);
//   SUCCEED();
// }

TEST_F(BinarySensor_test, getState_when_false) {
  EXPECT_EQ(bs.getState(), false);
}

TEST_F(BinarySensor_test, getState_when_true) {
  bs.setState(true);
  EXPECT_EQ(bs.getState(), true);
}

TEST_F(BinarySensor_test, getStateName_when_false) {
  EXPECT_STREQ(bs.getStateName(), "off");
}

TEST_F(BinarySensor_test, getStateName_when_true) {
  bs.setState(true);
  EXPECT_STREQ(bs.getStateName(), "on");
}

TEST_F(BinarySensor_test, getComponentType) {
  EXPECT_EQ(bs.getComponentType(), BaseComponent::Type::binarySensor);
}

TEST_F(BinarySensor_test, getDeviceClass) {
  EXPECT_EQ(bs.getDeviceClass(), BinarySensorDeviceClass::none);
}

TEST_F(BinarySensor_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(bs.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf,
      ElementsAre(34, static_cast<uint8_t>(BaseComponent::Type::binarySensor),
                  static_cast<uint8_t>(BinarySensorDeviceClass::none),
                  static_cast<uint8_t>(Unit::Type::none), (1 << 4) | 0));
}

TEST_F(BinarySensor_test, getValueMsg_when_false) {
  uint8_t buf[2] = {};
  EXPECT_EQ(bs.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, false));
}

TEST_F(BinarySensor_test, getValueMsg_when_true) {
  uint8_t buf[2] = {};
  bs.setState(true);
  EXPECT_EQ(bs.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, true));
}

TEST_F(BinarySensor_test, setReported) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis())
      .WillOnce(Return(0))
      .WillOnce(Return(10000))
      .WillOnce(Return(20500))
      .WillOnce(Return(35999));
  bs.setReported();
  EXPECT_EQ(bs.isDiffLastReportedState(), false);
  EXPECT_EQ(bs.timeSinceLastReport(), 10);
  bs.setState(true);
  EXPECT_EQ(bs.isDiffLastReportedState(), true);
  bs.setReported();
  EXPECT_EQ(bs.timeSinceLastReport(), 15);
  releaseArduinoMock();
}
