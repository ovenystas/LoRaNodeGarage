#include "Cover.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/Arduino.h"

using ::testing::ElementsAre;
using ::testing::Return;

class Cover_test : public ::testing::Test {
 protected:
  Cover cc = Cover(34, "Cover");
};

TEST_F(Cover_test, hasService_yes) { EXPECT_EQ(cc.hasService(), true); }

TEST_F(Cover_test, getState) {
  EXPECT_EQ(cc.getState(), CoverState::closed);
  cc.setState(CoverState::opening);
  EXPECT_EQ(cc.getState(), CoverState::opening);
  cc.setState(CoverState::open);
  EXPECT_EQ(cc.getState(), CoverState::open);
  cc.setState(CoverState::closing);
  EXPECT_EQ(cc.getState(), CoverState::closing);
}

TEST_F(Cover_test, getStateName_of_current_state) {
  EXPECT_STREQ(cc.getStateName(), "closed");
  cc.setState(CoverState::opening);
  EXPECT_STREQ(cc.getStateName(), "opening");
  cc.setState(CoverState::open);
  EXPECT_STREQ(cc.getStateName(), "open");
  cc.setState(CoverState::closing);
  EXPECT_STREQ(cc.getStateName(), "closing");
}

TEST_F(Cover_test, getStateName_of_state_as_arg) {
  EXPECT_STREQ(cc.getStateName(CoverState::closed), "closed");
  EXPECT_STREQ(cc.getStateName(CoverState::opening), "opening");
  EXPECT_STREQ(cc.getStateName(CoverState::open), "open");
  EXPECT_STREQ(cc.getStateName(CoverState::closing), "closing");
}

TEST_F(Cover_test, getServiceName_of_service_as_arg) {
  EXPECT_STREQ(cc.getServiceName(CoverService::close), "close");
  EXPECT_STREQ(cc.getServiceName(CoverService::open), "open");
  EXPECT_STREQ(cc.getServiceName(CoverService::stop), "stop");
  EXPECT_STREQ(cc.getServiceName(CoverService::toggle), "toggle");
}

TEST_F(Cover_test, getComponentType) {
  EXPECT_EQ(cc.getComponentType(), Component::Type::cover);
}

TEST_F(Cover_test, getDeviceClass) {
  EXPECT_EQ(cc.getDeviceClass(), CoverDeviceClass::none);
}

TEST_F(Cover_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(cc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(34, static_cast<uint8_t>(Component::Type::cover),
                       static_cast<uint8_t>(CoverDeviceClass::none),
                       static_cast<uint8_t>(Unit::Type::none), (1 << 4) | 0));
}

TEST_F(Cover_test, getValueMsg) {
  uint8_t buf[2] = {};
  EXPECT_EQ(cc.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, static_cast<uint8_t>(CoverState::closed)));
  cc.setState(CoverState::opening);
  EXPECT_EQ(cc.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, static_cast<uint8_t>(CoverState::opening)));
  cc.setState(CoverState::open);
  EXPECT_EQ(cc.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, static_cast<uint8_t>(CoverState::open)));
  cc.setState(CoverState::closing);
  EXPECT_EQ(cc.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, static_cast<uint8_t>(CoverState::closing)));
}

TEST_F(Cover_test, setReported) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis())
      .WillOnce(Return(0))
      .WillOnce(Return(10000))
      .WillOnce(Return(20500))
      .WillOnce(Return(35999));
  cc.setReported();
  EXPECT_EQ(cc.isDiffLastReportedState(), false);
  EXPECT_EQ(cc.timeSinceLastReport(), 10);
  cc.setState(CoverState::opening);
  EXPECT_EQ(cc.isDiffLastReportedState(), true);
  cc.setReported();
  EXPECT_EQ(cc.timeSinceLastReport(), 15);
  releaseArduinoMock();
}

// TEST_F(Cover_test, print) {
//   cc.print(Serial);
//   SUCCEED();
// }

// TEST_F(Cover_test, print) {
//   cc.print(Serial, 0);
//   SUCCEED();
// }
