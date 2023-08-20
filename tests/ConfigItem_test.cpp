#include "ConfigItem.h"

#include <gtest/gtest.h>

#include "Types.h"
#include "mocks/EEPROM.h"

#define EE_ADDR 321

class ConfigItem_test : public ::testing::Test {
 protected:
  void SetUp() override { eeprom_clear(); }
};

TEST_F(ConfigItem_test, constructDefault_uint8) {
  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(255, EE_ADDR);

  EXPECT_EQ(cfgItm.getPrecision(), 0);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::none);
  EXPECT_STREQ(cfgItm.getUnit().name(), "");
  EXPECT_EQ(cfgItm.getValue(), 0);
  EXPECT_EQ(cfgItm.getConfigId(), 255);
}

TEST_F(ConfigItem_test, constructDefault_int32) {
  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(255, EE_ADDR);

  EXPECT_EQ(cfgItm.getPrecision(), 0);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::none);
  EXPECT_STREQ(cfgItm.getUnit().name(), "");
  EXPECT_EQ(cfgItm.getValue(), 0);
  EXPECT_EQ(cfgItm.getConfigId(), 255);
}

TEST_F(ConfigItem_test, load_uint8_fail_shall_save_default_value) {
  eeprom_write_byte(EE_ADDR, 0xFF);      // Value
  eeprom_write_byte(EE_ADDR + 1, 0xFF);  // CRC
  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(255, EE_ADDR, 0xAA);

  cfgItm.load();

  EXPECT_EQ(cfgItm.getValue(), 0xAA);
  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 0xAA);      // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0x5F);  // CRC8 of value
}

TEST_F(ConfigItem_test, load_uint8_OK_shall_set_loaded_value) {
  eeprom_write_byte(EE_ADDR, 0xAA);      // Value
  eeprom_write_byte(EE_ADDR + 1, 0x5F);  // CRC
  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(255, EE_ADDR, 0xBB);

  cfgItm.load();

  EXPECT_EQ(cfgItm.getValue(), 0xAA);
  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 0xAA);      // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0x5F);  // CRC8 of value
}

TEST_F(ConfigItem_test, load_int32_fail_shall_save_default_value) {
  eeprom_write_byte(EE_ADDR, 0xC0);      // Value
  eeprom_write_byte(EE_ADDR + 1, 0xBD);  // Value
  eeprom_write_byte(EE_ADDR + 2, 0xF0);  // Value
  eeprom_write_byte(EE_ADDR + 3, 0xFF);  // Value
  eeprom_write_byte(EE_ADDR + 4, 0x00);  // Wrong CRC8 of value
  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(255, EE_ADDR, -1000000);

  cfgItm.load();

  EXPECT_EQ(cfgItm.getValue(), -1000000);
  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 0xC0);      // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0xBD);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 2), 0xF0);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 3), 0xFF);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 4), 0x55);  // CRC8 of value
}

TEST_F(ConfigItem_test, load_int32_OK_shall_set_loaded_value) {
  eeprom_write_byte(EE_ADDR, 0xC0);      // Value
  eeprom_write_byte(EE_ADDR + 1, 0xBD);  // Value
  eeprom_write_byte(EE_ADDR + 2, 0xF0);  // Value
  eeprom_write_byte(EE_ADDR + 3, 0xFF);  // Value
  eeprom_write_byte(EE_ADDR + 4, 0x55);  // Correct CRC8 of value
  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(255, EE_ADDR, -2000000);

  cfgItm.load();

  EXPECT_EQ(cfgItm.getValue(), -1000000);
  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 0xC0);      // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0xBD);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 2), 0xF0);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 3), 0xFF);  // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 4), 0x55);  // CRC8 of value
}

TEST_F(ConfigItem_test, load_uint8_ee_address_out_of_range_shall_do_nothing) {
  eeprom_write_byte(EE_ADDR, 0xFF);      // Value
  eeprom_write_byte(EE_ADDR + 1, 0xFF);  // CRC
  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(255, E2END + 1, 0xAA);

  cfgItm.load();

  EXPECT_EQ(cfgItm.getValue(), 0xAA);
  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 0xFF);      // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0xFF);  // CRC8 of value
}

TEST_F(ConfigItem_test, constructAllParams) {
  ConfigItem<uint8_t> cfgItm =
      ConfigItem<uint8_t>(123, EE_ADDR, 45, Unit::Type::percent, 3);

  EXPECT_EQ(cfgItm.getPrecision(), 3);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1000);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::percent);
  EXPECT_STREQ(cfgItm.getUnit().name(), "%");
  EXPECT_EQ(cfgItm.getValue(), 45);
  EXPECT_EQ(cfgItm.getConfigId(), 123);
}

TEST_F(ConfigItem_test, setValue) {
  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(33, EE_ADDR);

  cfgItm.setValue(111);

  EXPECT_EQ(cfgItm.getValue(), 111);

  EXPECT_EQ(eeprom_read_byte(EE_ADDR), 111);       // Value
  EXPECT_EQ(eeprom_read_byte(EE_ADDR + 1), 0x0A);  // CRC8 of value
}

TEST_F(ConfigItem_test, getDiscoveryConfigItem_default) {
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
  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(99, EE_ADDR);
  ConfigItemValueT item;

  cfgItm.getConfigItemValue(&item);

  EXPECT_EQ(item.configId, 99);
  EXPECT_EQ(item.value, 0);
}

TEST_F(ConfigItem_test, getConfigItemValue_0xDEADBEEF) {
  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(99, EE_ADDR, 0xDEADBEEF);

  ConfigItemValueT item;
  cfgItm.getConfigItemValue(&item);

  EXPECT_EQ(item.configId, 99);
  EXPECT_EQ(item.value, 0xDEADBEEF);
}

TEST_F(ConfigItem_test, setConfigItemValue) {
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
