#include "ValueItem.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/BufferSerial.h"

class ValueItem_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pSerial = new BufferSerial(256);
    strBuf[0] = '\0';
  }

  void TearDown() override { delete pSerial; }

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
};

TEST_F(ValueItem_test, construct_default_uint8) {
  auto v = ValueItem<uint8_t>();

  EXPECT_EQ(v.getValue(), 0);
  EXPECT_EQ(v.getValueSize(), 1);
  EXPECT_EQ(v.getUnit().type(), Unit::Type::none);
  EXPECT_EQ(v.getPrecision(), 0);
  EXPECT_EQ(v.getScaleFactor(), 1);
  EXPECT_FALSE(v.isSigned());
}

TEST_F(ValueItem_test, construct_default_int32) {
  auto v = ValueItem<int32_t>();

  EXPECT_EQ(v.getValue(), 0);
  EXPECT_EQ(v.getValueSize(), 4);
  EXPECT_EQ(v.getUnit().type(), Unit::Type::none);
  EXPECT_EQ(v.getPrecision(), 0);
  EXPECT_EQ(v.getScaleFactor(), 1);
  EXPECT_TRUE(v.isSigned());
}

TEST_F(ValueItem_test, construct_with_args_uint8) {
  auto v = ValueItem<uint8_t>(Unit::Type::C, 3, UINT8_MAX);

  EXPECT_EQ(v.getValue(), UINT8_MAX);
  EXPECT_EQ(v.getValueSize(), 1);
  EXPECT_EQ(v.getUnit().type(), Unit::Type::C);
  EXPECT_EQ(v.getPrecision(), 3);
  EXPECT_EQ(v.getScaleFactor(), 1000);
  EXPECT_FALSE(v.isSigned());
}

TEST_F(ValueItem_test, construct_with_args_int32) {
  auto v = ValueItem<int32_t>(Unit::Type::F, 3, INT32_MIN);

  EXPECT_EQ(v.getValue(), INT32_MIN);
  EXPECT_EQ(v.getValueSize(), 4);
  EXPECT_EQ(v.getUnit().type(), Unit::Type::F);
  EXPECT_EQ(v.getPrecision(), 3);
  EXPECT_EQ(v.getScaleFactor(), 1000);
  EXPECT_TRUE(v.isSigned());
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

TEST_F(ValueItem_test, print_uint32_max_val_precision_0_no_unit) {
  const char* expectStr = "4294967295";
  auto v = ValueItem<uint32_t>(Unit::Type::none, 0, UINT32_MAX);

  size_t printedChars = v.print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(ValueItem_test, print_uint32_max_val_precision_3_unit_C) {
  const char* expectStr = "4294967.295 °C";
  auto v = ValueItem<uint32_t>(Unit::Type::C, 3, UINT32_MAX);

  size_t printedChars = v.print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(ValueItem_test, print_int32_min_val_precision_2_unit_F) {
  const char* expectStr = "-21474836.48 °F";
  auto v = ValueItem<int32_t>(Unit::Type::F, 2, INT32_MIN);

  size_t printedChars = v.print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(ValueItem_test, print_int8_val_minus_1_precision_3_unit_km) {
  const char* expectStr = "-0.001 km";
  auto v = ValueItem<int8_t>(Unit::Type::km, 3, -1);

  size_t printedChars = v.print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(ValueItem_test, print_uint8_val_0_precision_3_unit_none) {
  const char* expectStr = "0.000";
  auto v = ValueItem<uint8_t>(Unit::Type::none, 3, 0);

  size_t printedChars = v.print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}
