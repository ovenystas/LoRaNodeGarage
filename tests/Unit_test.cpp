#include "../Unit.h"

#include <gtest/gtest.h>

#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"

class Unit_test : public ::testing::Test {
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

TEST_F(Unit_test, type_km) {
  Unit unit = Unit(Unit::Type::km);
  EXPECT_EQ(unit.type(), Unit::Type::km);
}

TEST_F(Unit_test, name_km) {
  Unit unit = Unit(Unit::Type::km);
  EXPECT_STREQ(unit.name(), "km");
}

TEST_F(Unit_test, print_unit_none) {
  const char* expectStr = "";
  Unit unit = Unit(Unit::Type::none);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Unit_test, print_unit_C) {
  const char* expectStr = " °C";
  Unit unit = Unit(Unit::Type::C);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Unit_test, print_unit_F) {
  const char* expectStr = " °F";
  Unit unit = Unit(Unit::Type::F);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Unit_test, print_unit_K) {
  const char* expectStr = " K";
  Unit unit = Unit(Unit::Type::K);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Unit_test, print_unit_percent) {
  const char* expectStr = " %";
  Unit unit = Unit(Unit::Type::percent);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Unit_test, print_unit_km) {
  const char* expectStr = " km";
  Unit unit = Unit(Unit::Type::km);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Unit_test, print_unit_m) {
  const char* expectStr = " m";
  Unit unit = Unit(Unit::Type::m);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Unit_test, print_unit_dm) {
  const char* expectStr = " dm";
  Unit unit = Unit(Unit::Type::dm);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Unit_test, print_unit_cm) {
  const char* expectStr = " cm";
  Unit unit = Unit(Unit::Type::cm);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Unit_test, print_unit_mm) {
  const char* expectStr = " mm";
  Unit unit = Unit(Unit::Type::mm);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Unit_test, print_unit_um) {
  const char* expectStr = " μm";
  Unit unit = Unit(Unit::Type::um);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Unit_test, print_unit_s) {
  const char* expectStr = " s";
  Unit unit = Unit(Unit::Type::s);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Unit_test, print_unit_ms) {
  const char* expectStr = " ms";
  Unit unit = Unit(Unit::Type::ms);

  size_t printedChars = unit.print(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}
