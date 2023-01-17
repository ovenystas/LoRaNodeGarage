#include "../Cover.h"

#include <gtest/gtest.h>

#include "../Unit.h"
#include "mocks/Arduino.h"

using ::testing::ElementsAre;
using ::testing::Return;

class CoverChild : public Cover {
 public:
  CoverChild(uint8_t entityId, const char* name) : Cover(entityId, name) {}

  virtual ~CoverChild() = default;

  void callService(Service service) override { return; }

  bool update() override { return false; }

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) override {
    buffer[0] = 0;
    return 0;
  }

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) override {
    return false;
  }
};

class Cover_test : public ::testing::Test {
 protected:
  CoverChild cc = CoverChild(34, "Cover");
};

TEST_F(Cover_test, hasService_yes) { EXPECT_EQ(cc.hasService(), true); }

// TEST_F(Cover_test, callService) {
//   cc.callService(static_cast<uint8_t>(0u));
//   SUCCEED();
// }

TEST_F(Cover_test, getState) {
  EXPECT_EQ(cc.getState(), Cover::State::closed);
  cc.setState(Cover::State::opening);
  EXPECT_EQ(cc.getState(), Cover::State::opening);
  cc.setState(Cover::State::open);
  EXPECT_EQ(cc.getState(), Cover::State::open);
  cc.setState(Cover::State::closing);
  EXPECT_EQ(cc.getState(), Cover::State::closing);
}

TEST_F(Cover_test, getStateName_of_current_state) {
  EXPECT_STREQ(cc.getStateName(), "closed");
  cc.setState(Cover::State::opening);
  EXPECT_STREQ(cc.getStateName(), "opening");
  cc.setState(Cover::State::open);
  EXPECT_STREQ(cc.getStateName(), "open");
  cc.setState(Cover::State::closing);
  EXPECT_STREQ(cc.getStateName(), "closing");
}

TEST_F(Cover_test, getStateName_of_state_as_arg) {
  EXPECT_STREQ(cc.getStateName(Cover::State::closed), "closed");
  EXPECT_STREQ(cc.getStateName(Cover::State::opening), "opening");
  EXPECT_STREQ(cc.getStateName(Cover::State::open), "open");
  EXPECT_STREQ(cc.getStateName(Cover::State::closing), "closing");
}

TEST_F(Cover_test, getServiceName_of_service_as_arg) {
  EXPECT_STREQ(cc.getServiceName(Cover::Service::close), "close");
  EXPECT_STREQ(cc.getServiceName(Cover::Service::open), "open");
  EXPECT_STREQ(cc.getServiceName(Cover::Service::stop), "stop");
  EXPECT_STREQ(cc.getServiceName(Cover::Service::toggle), "toggle");
}

TEST_F(Cover_test, getComponentType) {
  EXPECT_EQ(cc.getComponentType(), Component::Type::cover);
}

TEST_F(Cover_test, getDeviceClass) {
  EXPECT_EQ(cc.getDeviceClass(), Cover::DeviceClass::none);
}

TEST_F(Cover_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(cc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(34, static_cast<uint8_t>(Component::Type::cover),
                       static_cast<uint8_t>(Cover::DeviceClass::none),
                       static_cast<uint8_t>(Unit::Type::none), (1 << 4) | 0));
}

TEST_F(Cover_test, getValueMsg) {
  uint8_t buf[2] = {};
  EXPECT_EQ(cc.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, static_cast<uint8_t>(Cover::State::closed)));
  cc.setState(Cover::State::opening);
  EXPECT_EQ(cc.getValueMsg(buf), 2);
  EXPECT_THAT(buf,
              ElementsAre(34, static_cast<uint8_t>(Cover::State::opening)));
  cc.setState(Cover::State::open);
  EXPECT_EQ(cc.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, static_cast<uint8_t>(Cover::State::open)));
  cc.setState(Cover::State::closing);
  EXPECT_EQ(cc.getValueMsg(buf), 2);
  EXPECT_THAT(buf,
              ElementsAre(34, static_cast<uint8_t>(Cover::State::closing)));
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
  cc.setState(Cover::State::opening);
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
