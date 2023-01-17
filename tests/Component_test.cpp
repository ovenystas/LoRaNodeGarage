#include "../Component.h"

#include <gtest/gtest.h>

#include "mocks/Arduino.h"

using ::testing::Return;

class ComponentChild : public Component {
 public:
  explicit ComponentChild(uint8_t entityId) : Component(entityId) {}

  ComponentChild(uint8_t entityId, const char* name)
      : Component(entityId, name) {}

  virtual ~ComponentChild() = default;

  bool update() override { return false; }

  uint8_t getDiscoveryMsg(uint8_t* buffer) override {
    buffer[0] = 0;
    return 0;
  }

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) override {
    buffer[0] = 0;
    return 0;
  }

  virtual uint8_t getValueMsg(uint8_t* buffer) override {
    buffer[0] = 0;
    return 0;
  }

  bool hasService() override { return false; }

  void callService(uint8_t service) override { return; }

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) override {
    return false;
  }

  void print(Stream& stream, uint8_t service) override { return; }

  void print(Stream& stream) override { return; }
};

TEST(Component_test, getEntityId_constructor_1_param) {
  ComponentChild cmp = ComponentChild(23);
  EXPECT_EQ(cmp.getEntityId(), 23);
}

TEST(Component_test, getEntityId_constructor_2_params) {
  ComponentChild cmp = ComponentChild(23, "Hello");
  EXPECT_EQ(cmp.getEntityId(), 23);
}

TEST(Component_test, setReported_and_timeSinceLastReport) {
  ComponentChild cmp = ComponentChild(23);
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis())
      .WillOnce(Return(10000ul))
      .WillOnce(Return(20500ul))
      .WillOnce(Return(35999ul))
      .WillOnce(Return(135000ul));
  cmp.setReported();
  EXPECT_EQ(cmp.timeSinceLastReport(), 10);
  cmp.setReported();
  EXPECT_EQ(cmp.timeSinceLastReport(), 100);
  releaseArduinoMock();
}

TEST(Component_test, getName_when_no_name_is_set) {
  ComponentChild cmp = ComponentChild(23);
  EXPECT_STREQ(cmp.getName(), "");
}

TEST(Component_test, getName_when_empty_name_is_set) {
  ComponentChild cmp = ComponentChild(23, "");
  EXPECT_STREQ(cmp.getName(), "");
}

TEST(Component_test, getName_when_shortest_name_is_set) {
  ComponentChild cmp = ComponentChild(23, "H");
  EXPECT_STREQ(cmp.getName(), "H");
}

TEST(Component_test, getName_when_largest_name_is_set) {
  ComponentChild cmp = ComponentChild(23, "12345678901234567890");
  EXPECT_STREQ(cmp.getName(), "12345678901234567890");
}

TEST(Component_test, getName_when_too_large_name_is_set_shall_truncate) {
  ComponentChild cmp = ComponentChild(23, "123456789012345678901234567890");
  EXPECT_STREQ(cmp.getName(), "12345678901234567890");
}
