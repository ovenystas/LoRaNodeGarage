#include "../BinarySensor.h"

#include <gtest/gtest.h>

#include "../Unit.h"
#include "mocks/Arduino.h"

using ::testing::ElementsAre;
using ::testing::Return;

class BinarySensorChild : public BinarySensor {
 public:
  BinarySensorChild(uint8_t entityId, const char* name)
      : BinarySensor(entityId, name) {}

  virtual ~BinarySensorChild() = default;

  bool update() override { return false; }

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) override {
    buffer[0] = 0;
    return 0;
  }

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) override {
    return false;
  }
};

class BinarySensor_test : public ::testing::Test {
 protected:
  BinarySensorChild bsc = BinarySensorChild(34, "BinarySensor");
};

TEST_F(BinarySensor_test, hasService_no) { EXPECT_EQ(bsc.hasService(), false); }

TEST_F(BinarySensor_test, callService) {
  bsc.callService(0);
  SUCCEED();
}

// TEST_F(BinarySensor_test, print) {
//   bsc.print(Serial, 0);
//   SUCCEED();
// }

TEST_F(BinarySensor_test, getState_when_false) {
  EXPECT_EQ(bsc.getState(), false);
}

TEST_F(BinarySensor_test, getState_when_true) {
  bsc.setState(true);
  EXPECT_EQ(bsc.getState(), true);
}

TEST_F(BinarySensor_test, getStateName_when_false) {
  EXPECT_STREQ(bsc.getStateName(), "off");
}

TEST_F(BinarySensor_test, getStateName_when_true) {
  bsc.setState(true);
  EXPECT_STREQ(bsc.getStateName(), "on");
}

TEST_F(BinarySensor_test, getComponentType) {
  EXPECT_EQ(bsc.getComponentType(), Component::Type::binarySensor);
}

TEST_F(BinarySensor_test, getDeviceClass) {
  EXPECT_EQ(bsc.getDeviceClass(), BinarySensor::DeviceClass::none);
}

TEST_F(BinarySensor_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(bsc.getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(34, static_cast<uint8_t>(Component::Type::binarySensor),
                       static_cast<uint8_t>(BinarySensor::DeviceClass::none),
                       static_cast<uint8_t>(Unit::Type::none), (1 << 4) | 0));
}

TEST_F(BinarySensor_test, getValueMsg_when_false) {
  uint8_t buf[2] = {};
  EXPECT_EQ(bsc.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, false));
}

TEST_F(BinarySensor_test, getValueMsg_when_true) {
  uint8_t buf[2] = {};
  bsc.setState(true);
  EXPECT_EQ(bsc.getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, true));
}

TEST_F(BinarySensor_test, setReported) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis())
      .WillOnce(Return(0))
      .WillOnce(Return(10000))
      .WillOnce(Return(20500))
      .WillOnce(Return(35999));
  bsc.setReported();
  EXPECT_EQ(bsc.isDiffLastReportedState(), false);
  EXPECT_EQ(bsc.timeSinceLastReport(), 10);
  bsc.setState(true);
  EXPECT_EQ(bsc.isDiffLastReportedState(), true);
  bsc.setReported();
  EXPECT_EQ(bsc.timeSinceLastReport(), 15);
  releaseArduinoMock();
}
