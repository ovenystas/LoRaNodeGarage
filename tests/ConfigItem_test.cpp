#include "ConfigItem.h"

#include <gtest/gtest.h>

#include "Types.h"
#include "mocks/EEPROM.h"

#define EE_ADDR 321

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

class ConfigItem_test : public ::testing::Test {
 protected:
  void SetUp() override { pEepromMock = eepromStrictMockInstance(); }

  void TearDown() override { releaseEepromMock(); }

  StrictMock<EepromMock>* pEepromMock;
};

TEST_F(ConfigItem_test, constructDefault_uint8_eeLoadOk) {
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR)))
      .WillOnce(Return(0));
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR + 1)))
      .WillOnce(Return(0));

  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(255, EE_ADDR);

  EXPECT_EQ(cfgItm.getPrecision(), 0);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::none);
  EXPECT_STREQ(cfgItm.getUnit().name(), "");
  EXPECT_EQ(cfgItm.getValue(), 0);
  EXPECT_EQ(cfgItm.getConfigId(), 255);
}

TEST_F(ConfigItem_test, constructDefault_uint8_eeLoadFail) {
  // Read EE value
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR)))
      .WillOnce(Return(0xAA))   // Load
      .WillOnce(Return(0xAA));  // Save (update)
  // Read EE crc
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR + 1)))
      .WillOnce(Return(0xFF))   // Load
      .WillOnce(Return(0xFF));  // Save (update)
  // Write EE value
  EXPECT_CALL(*pEepromMock,
              eeprom_write_byte(reinterpret_cast<uint8_t*>(EE_ADDR), 0));
  // Write EE crc
  EXPECT_CALL(*pEepromMock,
              eeprom_write_byte(reinterpret_cast<uint8_t*>(EE_ADDR + 1), 0));

  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(255, EE_ADDR);

  EXPECT_EQ(cfgItm.getPrecision(), 0);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::none);
  EXPECT_STREQ(cfgItm.getUnit().name(), "");
  EXPECT_EQ(cfgItm.getValue(), 0);
  EXPECT_EQ(cfgItm.getConfigId(), 255);
}

TEST_F(ConfigItem_test, constructDefault_int32_eeLoadOk) {
  // Read EE value
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR)))
      .WillOnce(Return(0));
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR + 1)))
      .WillOnce(Return(0));
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR + 2)))
      .WillOnce(Return(0));
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR + 3)))
      .WillOnce(Return(0));
  // Read EE crc
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR + 4)))
      .WillOnce(Return(0));

  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(255, EE_ADDR);

  EXPECT_EQ(cfgItm.getPrecision(), 0);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::none);
  EXPECT_STREQ(cfgItm.getUnit().name(), "");
  EXPECT_EQ(cfgItm.getValue(), 0);
  EXPECT_EQ(cfgItm.getConfigId(), 255);
}

TEST_F(ConfigItem_test, constructDefault_int32_eeLoadFail) {
  // Read EE value
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR)))
      .WillOnce(Return(0xAA))   // Load
      .WillOnce(Return(0xAA));  // Save (update)
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR + 1)))
      .WillOnce(Return(0xAA))   // Load
      .WillOnce(Return(0xAA));  // Save (update)
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR + 2)))
      .WillOnce(Return(0xAA))   // Load
      .WillOnce(Return(0xAA));  // Save (update)
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR + 3)))
      .WillOnce(Return(0xAA))   // Load
      .WillOnce(Return(0xAA));  // Save (update)
  // Read EE crc
  EXPECT_CALL(*pEepromMock,
              eeprom_read_byte(reinterpret_cast<const uint8_t*>(EE_ADDR + 4)))
      .WillOnce(Return(0xFF))   // Load
      .WillOnce(Return(0xFF));  // Save (update)
  // Write EE value
  EXPECT_CALL(*pEepromMock,
              eeprom_write_byte(reinterpret_cast<uint8_t*>(EE_ADDR), 0));
  EXPECT_CALL(*pEepromMock,
              eeprom_write_byte(reinterpret_cast<uint8_t*>(EE_ADDR + 1), 0));
  EXPECT_CALL(*pEepromMock,
              eeprom_write_byte(reinterpret_cast<uint8_t*>(EE_ADDR + 2), 0));
  EXPECT_CALL(*pEepromMock,
              eeprom_write_byte(reinterpret_cast<uint8_t*>(EE_ADDR + 3), 0));
  // Write EE crc
  EXPECT_CALL(*pEepromMock,
              eeprom_write_byte(reinterpret_cast<uint8_t*>(EE_ADDR + 4), 0));

  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(255, EE_ADDR);

  EXPECT_EQ(cfgItm.getPrecision(), 0);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::none);
  EXPECT_STREQ(cfgItm.getUnit().name(), "");
  EXPECT_EQ(cfgItm.getValue(), 0);
  EXPECT_EQ(cfgItm.getConfigId(), 255);
}

TEST_F(ConfigItem_test, constructAllParams_eeLoadOk) {
  EXPECT_CALL(*pEepromMock, eeprom_read_byte(_))
      .WillOnce(Return(90))     // Value in ee is 90
      .WillOnce(Return(0x81));  // Crc of value 90

  ConfigItem<uint8_t> cfgItm =
      ConfigItem<uint8_t>(123, EE_ADDR, 45, Unit::Type::percent, 3);

  EXPECT_EQ(cfgItm.getPrecision(), 3);
  EXPECT_EQ(cfgItm.getScaleFactor(), 1000);
  EXPECT_EQ(cfgItm.getUnit().type(), Unit::Type::percent);
  EXPECT_STREQ(cfgItm.getUnit().name(), "%");
  EXPECT_EQ(cfgItm.getValue(), 90);  // Loaded from ee
  EXPECT_EQ(cfgItm.getConfigId(), 123);
}

TEST_F(ConfigItem_test, setValue) {
  EXPECT_CALL(*pEepromMock, eeprom_read_byte(_))
      .Times(2);  // Load value and crc as default zeroes

  ConfigItem<uint8_t> cfgItm = ConfigItem<uint8_t>(33, EE_ADDR);

  // Read EE on update
  EXPECT_CALL(*pEepromMock, eeprom_read_byte(_))
      .WillOnce(Return(0x00))   // Save value (update)
      .WillOnce(Return(0x00));  // Save crc (update)
  // Write EE value 111
  EXPECT_CALL(*pEepromMock, eeprom_write_byte(_, 111));
  // Write EE crc of 111
  EXPECT_CALL(*pEepromMock, eeprom_write_byte(_, 0x0A));

  cfgItm.setValue(111);

  EXPECT_EQ(cfgItm.getValue(), 111);
}

TEST_F(ConfigItem_test, getDiscoveryConfigItem_default) {
  EXPECT_CALL(*pEepromMock, eeprom_read_byte(_))
      .Times(5);  // Load value and crc as default zeroes
  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(99, EE_ADDR);
  DiscoveryConfigItemT item;

  cfgItm.getDiscoveryConfigItem(&item);

  EXPECT_EQ(item.configId, 99);
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_TRUE(item.isSigned);
  EXPECT_EQ(item.size, sizeof(int32_t) / 2);
  EXPECT_EQ(item.precision, 0);
}

TEST_F(ConfigItem_test, getDiscoveryConfigItem_Type_km_precision_3) {
  EXPECT_CALL(*pEepromMock, eeprom_read_byte(_))
      .Times(3);  // Load value and crc as default zeroes
  ConfigItem<uint16_t> cfgItm =
      ConfigItem<uint16_t>(78, EE_ADDR, 0, Unit::Type::km, 3);
  DiscoveryConfigItemT item;

  cfgItm.getDiscoveryConfigItem(&item);

  EXPECT_EQ(item.configId, 78);
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::km));
  EXPECT_FALSE(item.isSigned);
  EXPECT_EQ(item.size, sizeof(uint16_t) / 2);
  EXPECT_EQ(item.precision, 3);
}

TEST_F(ConfigItem_test, getDiscoveryConfigItem_Type_C_precision_1) {
  EXPECT_CALL(*pEepromMock, eeprom_read_byte(_))
      .Times(3);  // Load value and crc as default zeroes
  ConfigItem<int16_t> cfgItm =
      ConfigItem<int16_t>(79, EE_ADDR, 0, Unit::Type::C, 1);
  DiscoveryConfigItemT item;

  cfgItm.getDiscoveryConfigItem(&item);

  EXPECT_EQ(item.configId, 79);
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::C));
  EXPECT_TRUE(item.isSigned);
  EXPECT_EQ(item.size, sizeof(int16_t) / 2);
  EXPECT_EQ(item.precision, 1);
}

TEST_F(ConfigItem_test, getConfigItemValue_default) {
  EXPECT_CALL(*pEepromMock, eeprom_read_byte(_))
      .Times(5);  // Load value and crc as default zeroes
  ConfigItem<int32_t> cfgItm = ConfigItem<int32_t>(99, EE_ADDR);
  ConfigItemValueT item;

  cfgItm.getConfigItemValue(&item);

  EXPECT_EQ(item.configId, 99);
  EXPECT_EQ(item.value, 0);
}

TEST_F(ConfigItem_test, getConfigItemValue_0xDEADBEEF) {
  EXPECT_CALL(*pEepromMock, eeprom_read_byte(_))
      // Load value
      .WillOnce(Return(0x00))
      .WillOnce(Return(0x00))
      .WillOnce(Return(0x00))
      .WillOnce(Return(0x00))
      // Load crc that doesn't match value
      .WillOnce(Return(0xFF))
      // Save (update) value
      .WillOnce(Return(0x00))
      .WillOnce(Return(0x00))
      .WillOnce(Return(0x00))
      .WillOnce(Return(0x00))
      // Save (update) crc that doesn't match value
      .WillOnce(Return(0xFF));
  EXPECT_CALL(*pEepromMock, eeprom_write_byte(_, 0xDE));
  EXPECT_CALL(*pEepromMock, eeprom_write_byte(_, 0xAD));
  EXPECT_CALL(*pEepromMock, eeprom_write_byte(_, 0xBE));
  EXPECT_CALL(*pEepromMock, eeprom_write_byte(_, 0xEF));
  EXPECT_CALL(*pEepromMock, eeprom_write_byte(_, 0xCA));

  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(99, EE_ADDR, 0xDEADBEEFul);

  ConfigItemValueT item;
  cfgItm.getConfigItemValue(&item);

  EXPECT_EQ(item.configId, 99);
  EXPECT_EQ(item.value, 0xDEADBEEF);
}

TEST_F(ConfigItem_test, setConfigItemValue) {
  EXPECT_CALL(*pEepromMock, eeprom_read_byte(_))
      .Times(5);  // Load value and crc as default zeroes
  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(145, EE_ADDR);

  const ConfigItemValueT item = {145, 0xABBACAFE};

  EXPECT_CALL(*pEepromMock, eeprom_read_byte(_))
      .Times(5);  // Load value and crc as default zeroes during save (update)
  // Save value
  EXPECT_CALL(*pEepromMock, eeprom_write_byte(_, 0xAB));
  EXPECT_CALL(*pEepromMock, eeprom_write_byte(_, 0xBA));
  EXPECT_CALL(*pEepromMock, eeprom_write_byte(_, 0xCA));
  EXPECT_CALL(*pEepromMock, eeprom_write_byte(_, 0xFE));
  // Save crc of 0xABBACAFE
  EXPECT_CALL(*pEepromMock, eeprom_write_byte(_, 0x83));

  EXPECT_EQ(cfgItm.setConfigItemValue(&item), 1);
  EXPECT_EQ(cfgItm.getValue(), 0xABBACAFEul);
}

TEST_F(ConfigItem_test, setConfigItemValue_wrong_id) {
  EXPECT_CALL(*pEepromMock, eeprom_read_byte(_))
      .Times(5);  // Load value and crc as default zeroes
  ConfigItem<uint32_t> cfgItm = ConfigItem<uint32_t>(145, EE_ADDR);

  const ConfigItemValueT item = {1, 0xABBACAFE};
  EXPECT_EQ(cfgItm.setConfigItemValue(&item), 0);
  EXPECT_EQ(cfgItm.getValue(), 0x00000000ul);
}
