#include "BaseComponent.h"

#include <gtest/gtest.h>

#include "mocks/Arduino.h"

using ::testing::Return;

TEST(BaseComponent_test, getEntityId_constructor_1_param) {
  BaseComponent cmp = BaseComponent(23);
  EXPECT_EQ(cmp.getEntityId(), 23);
}

TEST(BaseComponent_test, getEntityId_constructor_2_params) {
  BaseComponent cmp = BaseComponent(23, "Hello");
  EXPECT_EQ(cmp.getEntityId(), 23);
}

TEST(BaseComponent_test, setReported_and_timeSinceLastReport) {
  BaseComponent cmp = BaseComponent(23);
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

TEST(BaseComponent_test, isReportDue) {
  BaseComponent cmp = BaseComponent(23);
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis()).WillOnce(Return(10000ul));

  // Newly constructed shall be true
  EXPECT_TRUE(cmp.isReportDue());

  // Shall be set to false when setReported() is called
  cmp.setReported();
  EXPECT_FALSE(cmp.isReportDue());

  // Shall set it with setIsReportDue()
  cmp.setIsReportDue(true);
  EXPECT_TRUE(cmp.isReportDue());
  cmp.setIsReportDue(false);
  EXPECT_FALSE(cmp.isReportDue());

  releaseArduinoMock();
}

TEST(BaseComponent_test, getName_when_no_name_is_set) {
  BaseComponent cmp = BaseComponent(23);
  EXPECT_STREQ(cmp.getName(), "");
}

TEST(BaseComponent_test, getName_when_empty_name_is_set) {
  BaseComponent cmp = BaseComponent(23, "");
  EXPECT_STREQ(cmp.getName(), "");
}

TEST(BaseComponent_test, getName_when_shortest_name_is_set) {
  BaseComponent cmp = BaseComponent(23, "H");
  EXPECT_STREQ(cmp.getName(), "H");
}

TEST(BaseComponent_test, getName_when_large_name_is_set_shall) {
  BaseComponent cmp = BaseComponent(23, "123456789012345678901234567890");
  EXPECT_STREQ(cmp.getName(), "123456789012345678901234567890");
}
