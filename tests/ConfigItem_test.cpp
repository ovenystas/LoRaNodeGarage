#include "ConfigItem.h"

#include <gtest/gtest.h>

using ::testing::ElementsAre;

TEST(ConfigItem_test, constructDefault) {
  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(255);

  EXPECT_EQ(cfgItm.getPrecision(), 0);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1);
  Unit unit = cfgItm.getUnit();
  EXPECT_EQ(unit.getType(), Unit::Type::none);
  EXPECT_STREQ(unit.getName(), "");
  EXPECT_EQ(cfgItm.getValue(), 0);
  EXPECT_EQ(cfgItm.getConfigId(), 255);
}

TEST(ConfigItem_test, constructAllParams) {
  ConfigItem<uint8_t> cfgItm =
      ConfigItem<uint8_t>(123, 45, Unit::Type::percent, 3);

  EXPECT_EQ(cfgItm.getPrecision(), 3);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1000);
  Unit unit = cfgItm.getUnit();
  EXPECT_EQ(unit.getType(), Unit::Type::percent);
  EXPECT_STREQ(unit.getName(), "%");
  EXPECT_EQ(cfgItm.getValue(), 45);
  EXPECT_EQ(cfgItm.getConfigId(), 123);
}

TEST(ConfigItem_test, setValue) {
  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(33);
  cfgItm.setValue(111);
  EXPECT_EQ(cfgItm.getValue(), 111);
}

TEST(ConfigItem_test, writeDiscoveryItem_default) {
  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(99);
  uint8_t buf[3] = {0};
  EXPECT_EQ(cfgItm.writeDiscoveryItem(buf), 3);
  EXPECT_THAT(buf, ElementsAre(99, static_cast<uint8_t>(Unit::Type::none),
                               (sizeof(int32_t) << 4 | 0)));
}

TEST(ConfigItem_test, writeDiscoveryItem_Type_km_precision_3) {
  ConfigItem<int16_t> cfgItm = ConfigItem<int16_t>(78, 0, Unit::Type::km, 3);
  uint8_t buf[3] = {0};
  EXPECT_EQ(cfgItm.writeDiscoveryItem(buf), 3);
  EXPECT_THAT(buf, ElementsAre(78, static_cast<uint8_t>(Unit::Type::km),
                               (sizeof(int16_t) << 4 | 3)));
}

TEST(ConfigItem_test, writeConfigItemValue_default) {
  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(99);
  uint8_t buf[1 + sizeof(int32_t)] = {0};
  EXPECT_EQ(cfgItm.writeConfigItemValue(buf), 1 + sizeof(int32_t));
  EXPECT_THAT(buf, ElementsAre(99, 0x00, 0x00, 0x00, 0x00));
}

TEST(ConfigItem_test, writeConfigItemValue_0xDEADBEEF) {
  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(99, 0xDEADBEEFul);
  uint8_t buf[1 + sizeof(uint32_t)] = {0};
  EXPECT_EQ(cfgItm.writeConfigItemValue(buf), 1 + sizeof(uint32_t));
  EXPECT_THAT(buf, ElementsAre(99, 0xDE, 0xAD, 0xBE, 0xEF));
}

TEST(ConfigItem_test, setConfigValue) {
  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(145);
  uint8_t buf[sizeof(uint32_t)] = {0xAB, 0xBA, 0xCA, 0xFE};
  EXPECT_EQ(cfgItm.setConfigValue(145, buf), 1 + sizeof(uint32_t));
  EXPECT_EQ(cfgItm.getValue(), 0xABBACAFEul);
}

TEST(ConfigItem_test, setConfigValue_wrong_id) {
  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(145);
  uint8_t buf[sizeof(uint32_t)] = {0xAB, 0xBA, 0xCA, 0xFE};
  EXPECT_EQ(cfgItm.setConfigValue(1, buf), 0);
  EXPECT_EQ(cfgItm.getValue(), 0x00000000ul);
}
