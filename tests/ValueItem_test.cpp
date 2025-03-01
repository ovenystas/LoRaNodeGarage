#include "ValueItem.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"

class ValueItem_test : public ::testing::Test {
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

using ValueItem_deathTest = ValueItem_test;

TEST_F(ValueItem_test, IS_SIGNED_TYPE) {
  EXPECT_FALSE(IS_SIGNED_TYPE(uint8_t));
  EXPECT_FALSE(IS_SIGNED_TYPE(uint16_t));
  EXPECT_FALSE(IS_SIGNED_TYPE(uint32_t));

  EXPECT_TRUE(IS_SIGNED_TYPE(int8_t));
  EXPECT_TRUE(IS_SIGNED_TYPE(int16_t));
  EXPECT_TRUE(IS_SIGNED_TYPE(int32_t));
}

TEST_F(ValueItem_test, SIGNED_MIN) {
  EXPECT_EQ(SIGNED_MIN(int8_t), INT8_MIN);
  EXPECT_EQ(SIGNED_MIN(int16_t), INT16_MIN);
  EXPECT_EQ(SIGNED_MIN(int32_t), INT32_MIN);
}

TEST_F(ValueItem_test, UNSIGNED_MIN) {
  EXPECT_EQ(UNSIGNED_MIN(uint8_t), 0);
  EXPECT_EQ(UNSIGNED_MIN(uint16_t), 0);
  EXPECT_EQ(UNSIGNED_MIN(uint32_t), 0);
}

TEST_F(ValueItem_test, SIGNED_MAX) {
  EXPECT_EQ(SIGNED_MAX(int8_t), INT8_MAX);
  EXPECT_EQ(SIGNED_MAX(int16_t), INT16_MAX);
  EXPECT_EQ(SIGNED_MAX(int32_t), INT32_MAX);
}

TEST_F(ValueItem_test, UNSIGNED_MAX) {
  EXPECT_EQ(UNSIGNED_MAX(uint8_t), UINT8_MAX);
  EXPECT_EQ(UNSIGNED_MAX(uint16_t), UINT16_MAX);
  EXPECT_EQ(UNSIGNED_MAX(uint32_t), UINT32_MAX);
}

TEST_F(ValueItem_test, MIN_OF) {
  EXPECT_EQ(MIN_OF(uint8_t), 0);
  EXPECT_EQ(MIN_OF(uint16_t), 0);
  EXPECT_EQ(MIN_OF(uint32_t), 0);

  EXPECT_EQ(MIN_OF(int8_t), INT8_MIN);
  EXPECT_EQ(MIN_OF(int16_t), INT16_MIN);
  EXPECT_EQ(MIN_OF(int32_t), INT32_MIN);
}

TEST_F(ValueItem_test, MAX_OF) {
  EXPECT_EQ(MAX_OF(uint8_t), UINT8_MAX);
  EXPECT_EQ(MAX_OF(uint16_t), UINT16_MAX);
  EXPECT_EQ(MAX_OF(uint32_t), UINT32_MAX);

  EXPECT_EQ(MAX_OF(int8_t), INT8_MAX);
  EXPECT_EQ(MAX_OF(int16_t), INT16_MAX);
  EXPECT_EQ(MAX_OF(int32_t), INT32_MAX);
}

TEST_F(ValueItem_test, construct_default_uint8) {
  auto v = ValueItem<uint8_t>();

  EXPECT_EQ(v.getValue(), 0);
  EXPECT_EQ(v.getValueSize(), 1);
  EXPECT_EQ(v.getUnit().type(), Unit::Type::none);
  EXPECT_EQ(v.getPrecision(), 0);
  EXPECT_EQ(v.getScaleFactor(), 1);
  EXPECT_FALSE(v.isSigned());
  EXPECT_EQ(v.getMinValue(), 0);
  EXPECT_EQ(v.getMaxValue(), UINT8_MAX);
}

TEST_F(ValueItem_test, construct_default_int32) {
  auto v = ValueItem<int32_t>();

  EXPECT_EQ(v.getValue(), 0);
  EXPECT_EQ(v.getValueSize(), 4);
  EXPECT_EQ(v.getUnit().type(), Unit::Type::none);
  EXPECT_EQ(v.getPrecision(), 0);
  EXPECT_EQ(v.getScaleFactor(), 1);
  EXPECT_TRUE(v.isSigned());
  EXPECT_EQ(v.getMinValue(), INT32_MIN);
  EXPECT_EQ(v.getMaxValue(), INT32_MAX);
}

TEST_F(ValueItem_test, construct_with_args_uint8) {
  auto v = ValueItem<uint8_t>(Unit::Type::C, 3, 12, 10, 15);

  EXPECT_EQ(v.getValue(), 12);
  EXPECT_EQ(v.getValueSize(), 1);
  EXPECT_EQ(v.getUnit().type(), Unit::Type::C);
  EXPECT_EQ(v.getPrecision(), 3);
  EXPECT_EQ(v.getScaleFactor(), 1000);
  EXPECT_FALSE(v.isSigned());
  EXPECT_EQ(v.getMinValue(), 10);
  EXPECT_EQ(v.getMaxValue(), 15);
}

TEST_F(ValueItem_test, construct_with_args_int32) {
  auto v = ValueItem<int32_t>(Unit::Type::F, 3, -123, -200, 100);

  EXPECT_EQ(v.getValue(), -123);
  EXPECT_EQ(v.getValueSize(), 4);
  EXPECT_EQ(v.getUnit().type(), Unit::Type::F);
  EXPECT_EQ(v.getPrecision(), 3);
  EXPECT_EQ(v.getScaleFactor(), 1000);
  EXPECT_TRUE(v.isSigned());
  EXPECT_EQ(v.getMinValue(), -200);
  EXPECT_EQ(v.getMaxValue(), 100);
}

TEST_F(ValueItem_deathTest, construct_with_value_less_than_min_uint8) {
  EXPECT_DEATH({ ValueItem<uint8_t>(Unit::Type::C, 3, 9, 10, 15); },
               "Assertion `mValue >= mMin' failed.");
}

TEST_F(ValueItem_deathTest, construct_with_value_greater_than_max_uint8) {
  EXPECT_DEATH({ ValueItem<uint8_t>(Unit::Type::C, 3, 16, 10, 15); },
               "Assertion `mValue <= mMax' failed.");
}

TEST_F(ValueItem_test, setValue) {
  auto v = ValueItem<int16_t>();
  EXPECT_EQ(v.getValue(), 0);

  v.setValue(INT16_MIN);
  EXPECT_EQ(v.getValue(), INT16_MIN);

  v.setValue(INT16_MAX);
  EXPECT_EQ(v.getValue(), INT16_MAX);

  v.setValue(0);
  EXPECT_EQ(v.getValue(), 0);
}

TEST_F(ValueItem_test, setValue_outside_range) {
  const int16_t MIN_VAL = -100;
  const int16_t MAX_VAL = 100;

  auto v = ValueItem<int16_t>(Unit::Type::C, 2, 0, MIN_VAL, MAX_VAL);
  EXPECT_EQ(v.getValue(), 0);

  v.setValue(MIN_VAL - 1);
  EXPECT_EQ(v.getValue(), MIN_VAL);

  v.setValue(MAX_VAL + 1);
  EXPECT_EQ(v.getValue(), MAX_VAL);
}

TEST_F(ValueItem_test, print_uint32_max_val_precision_0_no_unit) {
  const char* expectStr = "4294967295";
  auto v = ValueItem<uint32_t>(Unit::Type::none, 0, UINT32_MAX);

  size_t printedChars = v.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(ValueItem_test, print_uint32_max_val_precision_3_unit_C) {
  const char* expectStr = "4294967.295°C";
  auto v = ValueItem<uint32_t>(Unit::Type::C, 3, UINT32_MAX);

  size_t printedChars = v.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(ValueItem_test, print_int32_min_val_precision_2_unit_F) {
  const char* expectStr = "-21474836.48°F";
  auto v = ValueItem<int32_t>(Unit::Type::F, 2, INT32_MIN);

  size_t printedChars = v.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(ValueItem_test, print_int8_val_minus_1_precision_3_unit_km) {
  const char* expectStr = "-0.001km";
  auto v = ValueItem<int8_t>(Unit::Type::km, 3, -1);

  size_t printedChars = v.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(ValueItem_test, print_uint8_val_0_precision_3_unit_none) {
  const char* expectStr = "0.000";
  auto v = ValueItem<uint8_t>(Unit::Type::none, 3, 0);

  size_t printedChars = v.printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}
