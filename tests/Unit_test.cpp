#include "../Unit.h"

#include <gtest/gtest.h>

TEST(Unit_test, GetType_km) {
  Unit *pUnit = new Unit(Unit::Type::km);
  EXPECT_EQ(pUnit->getType(), Unit::Type::km);
  delete pUnit;
}

TEST(Unit_test, GetName_km) {
  Unit *pUnit = new Unit(Unit::Type::km);
  EXPECT_STREQ(pUnit->getName(), "km");
  delete pUnit;
}
