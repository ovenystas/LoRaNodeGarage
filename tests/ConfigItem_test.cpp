#include "ConfigItem.h"

#include <gtest/gtest.h>

#include "Types.h"

TEST(ConfigItem_test, constructDefault) {
  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(255);

  EXPECT_EQ(cfgItm.getPrecision(), 0);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1);
  EXPECT_EQ(cfgItm.getUnit().getType(), Unit::Type::none);
  EXPECT_STREQ(cfgItm.getUnit().getName(), "");
  EXPECT_EQ(cfgItm.getValue(), 0);
  EXPECT_EQ(cfgItm.getConfigId(), 255);
}

TEST(ConfigItem_test, constructAllParams) {
  ConfigItem<uint8_t> cfgItm =
      ConfigItem<uint8_t>(123, 45, Unit::Type::percent, 3);

  EXPECT_EQ(cfgItm.getPrecision(), 3);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1000);
  EXPECT_EQ(cfgItm.getUnit().getType(), Unit::Type::percent);
  EXPECT_STREQ(cfgItm.getUnit().getName(), "%");
  EXPECT_EQ(cfgItm.getValue(), 45);
  EXPECT_EQ(cfgItm.getConfigId(), 123);
}

TEST(ConfigItem_test, setValue) {
  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(33);

  cfgItm.setValue(111);

  EXPECT_EQ(cfgItm.getValue(), 111);
}

TEST(ConfigItem_test, getDiscoveryConfigItem_default) {
  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(99);
  DiscoveryConfigItemT item;

  cfgItm.getDiscoveryConfigItem(&item);

  EXPECT_EQ(item.configId, 99);
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_EQ(item.size, sizeof(int32_t));
  EXPECT_EQ(item.precision, 0);
}

TEST(ConfigItem_test, getDiscoveryConfigItem_Type_km_precision_3) {
  ConfigItem<int16_t> cfgItm = ConfigItem<int16_t>(78, 0, Unit::Type::km, 3);
  DiscoveryConfigItemT item;

  cfgItm.getDiscoveryConfigItem(&item);

  EXPECT_EQ(item.configId, 78);
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::km));
  EXPECT_EQ(item.size, sizeof(int16_t));
  EXPECT_EQ(item.precision, 3);
}

TEST(ConfigItem_test, getConfigItemValue_default) {
  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(99);
  ConfigItemValueT item;

  cfgItm.getConfigItemValue(&item);

  EXPECT_EQ(item.configId, 99);
  EXPECT_EQ(item.value, 0);
}

TEST(ConfigItem_test, getConfigItemValue_0xDEADBEEF) {
  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(99, 0xDEADBEEFul);
  ConfigItemValueT item;

  cfgItm.getConfigItemValue(&item);

  EXPECT_EQ(item.configId, 99);
  EXPECT_EQ(item.value, 0xDEADBEEF);
}

TEST(ConfigItem_test, setConfigItemValue) {
  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(145);
  const ConfigItemValueT item = {145, 0xABBACAFE};
  EXPECT_EQ(cfgItm.setConfigItemValue(&item), 1);
  EXPECT_EQ(cfgItm.getValue(), 0xABBACAFEul);
}

TEST(ConfigItem_test, setConfigItemValue_wrong_id) {
  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(145);
  const ConfigItemValueT item = {1, 0xABBACAFE};
  EXPECT_EQ(cfgItm.setConfigItemValue(&item), 0);
  EXPECT_EQ(cfgItm.getValue(), 0x00000000ul);
}
