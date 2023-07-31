#include "../Unit.h"

#include <gtest/gtest.h>

TEST(Unit_test, type_km) {
  Unit unit = Unit(Unit::Type::km);
  EXPECT_EQ(unit.type(), Unit::Type::km);
}

TEST(Unit_test, name_km) {
  Unit unit = Unit(Unit::Type::km);
  EXPECT_STREQ(unit.name(), "km");
}
