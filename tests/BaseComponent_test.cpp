#include "BaseComponent.h"

#include <gtest/gtest.h>

#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"

using ::testing::Return;

class BaseComponent_test : public ::testing::Test {
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

TEST_F(BaseComponent_test, getEntityId_constructor_1_param) {
  BaseComponent cmp = BaseComponent(23);
  EXPECT_EQ(cmp.getEntityId(), 23);
}

TEST_F(BaseComponent_test, getEntityId_constructor_2_params) {
  BaseComponent cmp = BaseComponent(23, "Hello");
  EXPECT_EQ(cmp.getEntityId(), 23);
}

TEST_F(BaseComponent_test, setReported_and_timeSinceLastReport) {
  BaseComponent cmp = BaseComponent(23);
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis())
      .WillOnce(Return(10000ul))
      .WillOnce(Return(20500ul))
      .WillOnce(Return(35999ul))
      .WillOnce(Return(135000ul));

  cmp.setReported();
  EXPECT_EQ(cmp.timeSinceLastReport(), 20500 - 10000);

  cmp.setReported();
  EXPECT_EQ(cmp.timeSinceLastReport(), 135000 - 35999);

  releaseArduinoMock();
}

TEST_F(BaseComponent_test, isReportDue) {
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

TEST_F(BaseComponent_test, print_name_when_no_name_is_set) {
  BaseComponent cmp = BaseComponent(23);
  const char* expectStr = "";

  size_t printedChars = cmp.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(BaseComponent_test, print_name_when_empty_name_is_set) {
  const char* expectStr = "";
  BaseComponent cmp = BaseComponent(23, expectStr);

  size_t printedChars = cmp.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(BaseComponent_test, print_name_when_shortest_name_is_set) {
  const char* expectStr = "B";
  BaseComponent cmp = BaseComponent(23, expectStr);

  size_t printedChars = cmp.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(BaseComponent_test, print_name_when_large_name_is_set) {
  const char* expectStr = "123456789012345678901234567890";
  BaseComponent cmp = BaseComponent(23, expectStr);

  size_t printedChars = cmp.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}
