#include "ConfigItem.h"

#include <gtest/gtest.h>

#include "Types.h"
#include "mocks/EEPROM.h"

#define EE_ADDR 321

class ConfigItem_test : public ::testing::Test {
 protected:
  void SetUp() override { eeprom_clear(); }
};

TEST_F(ConfigItem_test, constructDefault_uint8_eeLoadOk) {
  eeprom_write_byte(EE_ADDR, 0x00);
  eeprom_write_byte(EE_ADDR + 1, 0x00);

  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(255, EE_ADDR);

  EXPECT_EQ(cfgItm.getPrecision(), 0);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::none);
  EXPECT_STREQ(cfgItm.getUnit().name(), "");
  EXPECT_EQ(cfgItm.getValue(), 0);
  EXPECT_EQ(cfgItm.getConfigId(), 255);
}

TEST_F(ConfigItem_test, constructDefault_uint8_eeLoadFail) {
  eeprom_write_byte(EE_ADDR, 0xAA);
  eeprom_write_byte(EE_ADDR + 1, 0xFF);

  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(255, EE_ADDR);

  EXPECT_EQ(cfgItm.getPrecision(), 0);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::none);
  EXPECT_STREQ(cfgItm.getUnit().name(), "");
  EXPECT_EQ(cfgItm.getValue(), 0);
  EXPECT_EQ(cfgItm.getConfigId(), 255);

  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 0x00);      // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0x00);  // CRC8 of value
}

TEST_F(ConfigItem_test, constructDefault_int32_eeLoadOk) {
  eeprom_write_byte(EE_ADDR, 0x00);
  eeprom_write_byte(EE_ADDR + 1, 0x00);
  eeprom_write_byte(EE_ADDR + 2, 0x00);
  eeprom_write_byte(EE_ADDR + 3, 0x00);
  eeprom_write_byte(EE_ADDR + 4, 0x00);

  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(255, EE_ADDR);

  EXPECT_EQ(cfgItm.getPrecision(), 0);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::none);
  EXPECT_STREQ(cfgItm.getUnit().name(), "");
  EXPECT_EQ(cfgItm.getValue(), 0);
  EXPECT_EQ(cfgItm.getConfigId(), 255);

  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 0x00);      // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0x00);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 2), 0x00);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 3), 0x00);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 4), 0x00);  // CRC8 of value
}

TEST_F(ConfigItem_test, constructDefault_int32_eeLoadFail) {
  eeprom_write_byte(EE_ADDR, 0xAA);
  eeprom_write_byte(EE_ADDR + 1, 0xAA);
  eeprom_write_byte(EE_ADDR + 2, 0xAA);
  eeprom_write_byte(EE_ADDR + 3, 0xAA);
  eeprom_write_byte(EE_ADDR + 4, 0xFF);

  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(255, EE_ADDR);

  EXPECT_EQ(cfgItm.getPrecision(), 0);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::none);
  EXPECT_STREQ(cfgItm.getUnit().name(), "");
  EXPECT_EQ(cfgItm.getValue(), 0);
  EXPECT_EQ(cfgItm.getConfigId(), 255);

  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 0x00);      // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0x00);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 2), 0x00);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 3), 0x00);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 4), 0x00);  // CRC8 of value
}

TEST_F(ConfigItem_test, constructAllParams_eeLoadOk) {
  eeprom_write_byte(EE_ADDR, 90);
  eeprom_write_byte(EE_ADDR + 1, 0x81);

  ConfigItem<uint8_t> cfgItm =
      ConfigItem<uint8_t>(123, EE_ADDR, 45, Unit::Type::percent, 3);

  EXPECT_EQ(cfgItm.getPrecision(), 3);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1000);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::percent);
  EXPECT_STREQ(cfgItm.getUnit().name(), "%");
  EXPECT_EQ(cfgItm.getValue(), 90);  // Loaded from ee
  EXPECT_EQ(cfgItm.getConfigId(), 123);

  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 90);        // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0x81);  // CRC8 of value
}

TEST_F(ConfigItem_test, setValue) {
  eeprom_write_byte(EE_ADDR, 0x00);
  eeprom_write_byte(EE_ADDR + 1, 0x00);

  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(33, EE_ADDR);

  cfgItm.setValue(111);

  EXPECT_EQ(cfgItm.getValue(), 111);

  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 111);       // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0x0A);  // CRC8 of value
}

TEST_F(ConfigItem_test, getDiscoveryConfigItem_default) {
  eeprom_write_byte(EE_ADDR, 0x00);
  eeprom_write_byte(EE_ADDR + 1, 0x00);
  eeprom_write_byte(EE_ADDR + 2, 0x00);
  eeprom_write_byte(EE_ADDR + 3, 0x00);
  eeprom_write_byte(EE_ADDR + 4, 0x00);
  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(99, EE_ADDR);
  DiscoveryConfigItemT item;

  cfgItm.getDiscoveryConfigItem(&item);

  EXPECT_EQ(item.configId, 99);
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_TRUE(item.isSigned);
  EXPECT_EQ(item.sizeCode, sizeof(int32_t) / 2);
  EXPECT_EQ(item.precision, 0);
}

TEST_F(ConfigItem_test, getDiscoveryConfigItem_Type_km_precision_3) {
  eeprom_write_byte(EE_ADDR, 0x00);
  eeprom_write_byte(EE_ADDR + 1, 0x00);
  eeprom_write_byte(EE_ADDR + 2, 0x00);
  eeprom_write_byte(EE_ADDR + 3, 0x00);
  eeprom_write_byte(EE_ADDR + 4, 0x00);
  ConfigItem<uint16_t> cfgItm =
      ConfigItem<uint16_t>(78, EE_ADDR, 0, Unit::Type::km, 3);
  DiscoveryConfigItemT item;

  cfgItm.getDiscoveryConfigItem(&item);

  EXPECT_EQ(item.configId, 78);
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::km));
  EXPECT_FALSE(item.isSigned);
  EXPECT_EQ(item.sizeCode, sizeof(uint16_t) / 2);
  EXPECT_EQ(item.precision, 3);
}

TEST_F(ConfigItem_test, getDiscoveryConfigItem_Type_C_precision_1) {
  eeprom_write_byte(EE_ADDR, 0x00);
  eeprom_write_byte(EE_ADDR + 1, 0x00);
  eeprom_write_byte(EE_ADDR + 2, 0x00);
  ConfigItem<int16_t> cfgItm =
      ConfigItem<int16_t>(79, EE_ADDR, 0, Unit::Type::C, 1);
  DiscoveryConfigItemT item;

  cfgItm.getDiscoveryConfigItem(&item);

  EXPECT_EQ(item.configId, 79);
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::C));
  EXPECT_TRUE(item.isSigned);
  EXPECT_EQ(item.sizeCode, sizeof(int16_t) / 2);
  EXPECT_EQ(item.precision, 1);
}

TEST_F(ConfigItem_test, getConfigItemValue_default) {
  eeprom_write_byte(EE_ADDR, 0x00);
  eeprom_write_byte(EE_ADDR + 1, 0x00);
  eeprom_write_byte(EE_ADDR + 2, 0x00);
  eeprom_write_byte(EE_ADDR + 3, 0x00);
  eeprom_write_byte(EE_ADDR + 4, 0x00);
  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(99, EE_ADDR);
  ConfigItemValueT item;

  cfgItm.getConfigItemValue(&item);

  EXPECT_EQ(item.configId, 99);
  EXPECT_EQ(item.value, 0);
}

TEST_F(ConfigItem_test, getConfigItemValue_0xDEADBEEF) {
  eeprom_write_byte(EE_ADDR, 0xEF);  // Little endian
  eeprom_write_byte(EE_ADDR + 1, 0xBE);
  eeprom_write_byte(EE_ADDR + 2, 0xAD);
  eeprom_write_byte(EE_ADDR + 3, 0xDE);
  eeprom_write_byte(EE_ADDR + 4, 0xCA);
  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(99, EE_ADDR, 0xDEADBEEF);

  ConfigItemValueT item;
  cfgItm.getConfigItemValue(&item);

  EXPECT_EQ(item.configId, 99);
  EXPECT_EQ(item.value, 0xDEADBEEF);
}

TEST_F(ConfigItem_test, setConfigItemValue) {
  eeprom_write_byte(EE_ADDR, 0x00);
  eeprom_write_byte(EE_ADDR + 1, 0x00);
  eeprom_write_byte(EE_ADDR + 2, 0x00);
  eeprom_write_byte(EE_ADDR + 3, 0x00);
  eeprom_write_byte(EE_ADDR + 4, 0x00);
  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(145, EE_ADDR);

  const ConfigItemValueT item = {145, 0xABBACAFE};
  EXPECT_EQ(cfgItm.setConfigItemValue(&item), 1);

  EXPECT_EQ(cfgItm.getValue(), 0xABBACAFEul);

  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 0xFE);      // Value in little endian
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0xCA);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 2), 0xBA);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 3), 0xAB);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 4), 0x83);  // CRC8 of value
}

TEST_F(ConfigItem_test, setConfigItemValue_wrong_id) {
  eeprom_write_byte(EE_ADDR, 0x00);
  eeprom_write_byte(EE_ADDR + 1, 0x00);
  eeprom_write_byte(EE_ADDR + 2, 0x00);
  eeprom_write_byte(EE_ADDR + 3, 0x00);
  eeprom_write_byte(EE_ADDR + 4, 0x00);
  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(145, EE_ADDR);

  const ConfigItemValueT item = {1, 0xABBACAFE};
  EXPECT_EQ(cfgItm.setConfigItemValue(&item), 0);

  EXPECT_EQ(cfgItm.getValue(), 0x00000000ul);

  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 0x00);      // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0x00);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 2), 0x00);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 3), 0x00);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 4), 0x00);  // CRC8 of value
}
