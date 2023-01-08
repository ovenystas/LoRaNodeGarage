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

  void setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) override {
    return;
  }
};

class Cover_test : public ::testing::Test {
 protected:
  CoverChild cs = CoverChild(34, "Cover");
};

TEST_F(Cover_test, hasService_yes) { EXPECT_EQ(cs.hasService(), true); }

// TEST_F(Cover_test, callService) {
//   cs.callService(static_cast<uint8_t>(0u));
//   SUCCEED();
// }

TEST_F(Cover_test, getState) {
  EXPECT_EQ(cs.getState(), Cover::State::closed);
  cs.setState(Cover::State::opening);
  EXPECT_EQ(cs.getState(), Cover::State::opening);
  cs.setState(Cover::State::open);
  EXPECT_EQ(cs.getState(), Cover::State::open);
  cs.setState(Cover::State::closing);
  EXPECT_EQ(cs.getState(), Cover::State::closing);
}

TEST_F(Cover_test, getStateName_of_current_state) {
  EXPECT_STREQ(cs.getStateName(), "closed");
  cs.setState(Cover::State::opening);
  EXPECT_STREQ(cs.getStateName(), "opening");
  cs.setState(Cover::State::open);
  EXPECT_STREQ(cs.getStateName(), "open");
  cs.setState(Cover::State::closing);
  EXPECT_STREQ(cs.getStateName(), "closing");
}

TEST_F(Cover_test, getStateName_of_state_as_arg) {
  EXPECT_STREQ(cs.getStateName(Cover::State::closed), "closed");
  EXPECT_STREQ(cs.getStateName(Cover::State::opening), "opening");
  EXPECT_STREQ(cs.getStateName(Cover::State::open), "open");
  EXPECT_STREQ(cs.getStateName(Cover::State::closing), "closing");
}

TEST_F(Cover_test, getServiceName_of_service_as_arg) {
  EXPECT_STREQ(cs.getServiceName(Cover::Service::close), "close");
  EXPECT_STREQ(cs.getServiceName(Cover::Service::open), "open");
  EXPECT_STREQ(cs.getServiceName(Cover::Service::stop), "stop");
  EXPECT_STREQ(cs.getServiceName(Cover::Service::toggle), "toggle");
}

TEST_F(Cover_test, getComponentType) {
  EXPECT_EQ(cs.getComponentType(), Component::Type::cover);
}

TEST_F(Cover_test, getDeviceClass) {
  EXPECT_EQ(cs.getDeviceClass(), Cover::DeviceClass::none);
}

TEST_F(Cover_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(cs.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(34, static_cast<uint8_t>(Component::Type::cover),
                       static_cast<uint8_t>(Cover::DeviceClass::none),
                       static_cast<uint8_t>(Unit::Type::none), (1 << 4) | 0));
}

TEST_F(Cover_test, getValueMsg) {
  uint8_t buf[2] = {};
  EXPECT_EQ(cs.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, static_cast<uint8_t>(Cover::State::closed)));
  cs.setState(Cover::State::opening);
  EXPECT_EQ(cs.getValueMsg(buf), 2);
  EXPECT_THAT(buf,
              ElementsAre(34, static_cast<uint8_t>(Cover::State::opening)));
  cs.setState(Cover::State::open);
  EXPECT_EQ(cs.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, static_cast<uint8_t>(Cover::State::open)));
  cs.setState(Cover::State::closing);
  EXPECT_EQ(cs.getValueMsg(buf), 2);
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
  cs.setReported();
  EXPECT_EQ(cs.isDiffLastReportedState(), false);
  EXPECT_EQ(cs.timeSinceLastReport(), 10);
  cs.setState(Cover::State::opening);
  EXPECT_EQ(cs.isDiffLastReportedState(), true);
  cs.setReported();
  EXPECT_EQ(cs.timeSinceLastReport(), 15);
  releaseArduinoMock();
}

// TEST_F(Cover_test, print) {
//   cs.print(Serial);
//   SUCCEED();
// }

// TEST_F(Cover_test, print) {
//   cs.print(Serial, 0);
//   SUCCEED();
// }
