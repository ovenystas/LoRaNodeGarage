#include "Types.h"

#include <gtest/gtest.h>

using ::testing::ElementsAre;

#define DISCOVERY_ENTITY_ITEM \
  { 1, 2, 3, 4, 3, 2, 1, 0 }
#define CONFIG_ITEM \
  { 1, 2, 3, 2, 1, 0, 123456, 654321 }
#define DISCOVERY_ITEM_TWO_CONFIG_ITEMS                                        \
  {                                                                            \
    1, 2, 3, 4, 3, 2, 1, 0, 2, {                                               \
      {0, 1, 3, 2, 1, 0, 123456, 654321}, { 1, 4, 3, 2, 1, 0, 123456, 654321 } \
    }                                                                          \
  }
#define BE_HEX_OF_123456 0x00, 0x01, 0xE2, 0x40
#define BE_HEX_OF_654321 0x00, 0x09, 0xFB, 0xF1

TEST(Type_test, DiscoveryEntityItemT_equal) {
  const DiscoveryEntityItemT item1 = DISCOVERY_ENTITY_ITEM;
  const DiscoveryEntityItemT item2 = item1;

  EXPECT_EQ(item1, item2);
}

TEST(Type_test, DiscoveryEntityItemT_not_equal) {
  const DiscoveryEntityItemT item1 = DISCOVERY_ENTITY_ITEM;
  DiscoveryEntityItemT item2 = item1;
  item2.precision++;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, DiscoveryEntityItemT_toByteArray) {
  const DiscoveryEntityItemT item1 = DISCOVERY_ENTITY_ITEM;
  uint8_t actual[5]{};

  size_t size = item1.toByteArray(actual, sizeof(actual));

  EXPECT_GT(size, 0);
  EXPECT_THAT(actual, ElementsAre(1, 2, 3, 4, 0x1B));
  EXPECT_EQ(size, 5);
}

TEST(Type_test, DiscoveryEntityItemT_toByteArray_tooSmallBuf) {
  const DiscoveryEntityItemT item1 = DISCOVERY_ENTITY_ITEM;
  uint8_t actual[5]{};

  size_t size = item1.toByteArray(actual, sizeof(actual) - 1);

  EXPECT_EQ(size, 0);
  EXPECT_THAT(actual, ElementsAre(0, 0, 0, 0, 0));
}

TEST(Type_test, DiscoveryConfigItemT_size) {
  const DiscoveryConfigItemT item = CONFIG_ITEM;

  EXPECT_EQ(item.size(), 11);
}

TEST(Type_test, DiscoveryConfigItemT_equal) {
  const DiscoveryConfigItemT item1 = CONFIG_ITEM;
  const DiscoveryConfigItemT item2 = item1;

  EXPECT_EQ(item1, item2);
}

TEST(Type_test, DiscoveryConfigItemT_not_equal) {
  const DiscoveryConfigItemT item1 = CONFIG_ITEM;
  DiscoveryConfigItemT item2 = item1;
  item2.precision++;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, DiscoveryConfigItemT_toByteArray) {
  const DiscoveryConfigItemT item1 = CONFIG_ITEM;
  uint8_t actual[11]{};

  size_t size = item1.toByteArray(actual, sizeof(actual));

  EXPECT_GT(size, 0);
  EXPECT_THAT(actual,
              ElementsAre(1, 2, 0x1B, BE_HEX_OF_123456, BE_HEX_OF_654321));
  EXPECT_EQ(size, 11);
}

TEST(Type_test, DiscoveryConfigItemT_toByteArray_tooSmallBuf) {
  const DiscoveryConfigItemT item1 = CONFIG_ITEM;
  uint8_t actual[11]{};

  size_t size = item1.toByteArray(actual, sizeof(actual) - 1);

  EXPECT_EQ(size, 0);
  EXPECT_THAT(actual, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Type_test, DiscoveryItemT_equal) {
  const DiscoveryItemT item1 = DISCOVERY_ITEM_TWO_CONFIG_ITEMS;
  const DiscoveryItemT item2 = item1;

  EXPECT_EQ(item1, item2);
}

TEST(Type_test, DiscoveryItemT_equal_ignore_configItems_outOfRange) {
  DiscoveryItemT item1 = DISCOVERY_ITEM_TWO_CONFIG_ITEMS;
  item1.numberOfConfigItems = 1;
  DiscoveryItemT item2 = item1;
  item2.configItems[1].precision++;

  EXPECT_EQ(item1, item2);
}

TEST(Type_test, DiscoveryItemT_entity_not_equal) {
  const DiscoveryItemT item1 = DISCOVERY_ITEM_TWO_CONFIG_ITEMS;
  DiscoveryItemT item2 = item1;
  item2.entity.sizeCode++;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, DiscoveryItemT_numberOfConfigItems_not_equal) {
  const DiscoveryItemT item1 = DISCOVERY_ITEM_TWO_CONFIG_ITEMS;
  DiscoveryItemT item2 = item1;
  item2.numberOfConfigItems++;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, DiscoveryItemT_numberOfConfigItems_tooLarge) {
  const DiscoveryItemT item1 = {
      1,
      2,
      3,
      4,
      3,
      2,
      1,
      0,
      14,
      {{0, 1, 3, 2, 1, 0, 123456, 654321}, {1, 4, 3, 2, 1, 0, 123456, 654321}}};
  DiscoveryItemT item2 = item1;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, DiscoveryItemT_configItems_not_equal) {
  const DiscoveryItemT item1 = {
      1,
      2,
      3,
      4,
      3,
      2,
      1,
      0,
      13,
      {{0, 1, 3, 2, 1, 0, 123456, 654321}, {1, 4, 3, 2, 1, 0, 123456, 654321}}};
  DiscoveryItemT item2 = item1;
  item2.configItems[2].sizeCode++;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, DiscoveryItemT_toByteArray) {
  const DiscoveryItemT item1 = DISCOVERY_ITEM_TWO_CONFIG_ITEMS;
  uint8_t actual[28]{};

  size_t size = item1.toByteArray(actual, sizeof(actual));

  EXPECT_GT(size, 0);
  EXPECT_EQ(actual[17], 1);
  EXPECT_THAT(actual, ElementsAre(1, 2, 3, 4, 0x1B, 2, 0, 1, 0x1B,
                                  BE_HEX_OF_123456, BE_HEX_OF_654321, 1, 4,
                                  0x1B, BE_HEX_OF_123456, BE_HEX_OF_654321));
  EXPECT_EQ(size, 28);
}

TEST(Type_test, DiscoveryItemT_toByteArray_tooSmallBuf) {
  const DiscoveryItemT item1 = DISCOVERY_ITEM_TWO_CONFIG_ITEMS;
  uint8_t actual[12]{};

  size_t size = item1.toByteArray(actual, sizeof(actual) - 1);

  EXPECT_EQ(size, 0);
  EXPECT_THAT(actual, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(Type_test, ValueItemT_equal) {
  const ValueItemT item1 = {1, 0x11223344};
  const ValueItemT item2 = item1;

  EXPECT_EQ(item1, item2);
}

TEST(Type_test, ValueItemT_not_equal) {
  const ValueItemT item1 = {1, 0x11223344};
  ValueItemT item2 = item1;
  item2.entityId++;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, ValueItemT_toByteArray) {
  const ValueItemT item1 = {1, 0x11223344};
  uint8_t actual[5]{};

  size_t size = item1.toByteArray(actual, sizeof(actual));

  EXPECT_GT(size, 0);
  EXPECT_THAT(actual, ElementsAre(1, 0x11, 0x22, 0x33, 0x44));
  EXPECT_EQ(size, 5);
}

TEST(Type_test, ValueItemT_toByteArray_tooSmallBuf) {
  const ValueItemT item1 = {1, 0x11223344};
  uint8_t actual[5]{};

  size_t size = item1.toByteArray(actual, sizeof(actual) - 1);

  EXPECT_EQ(size, 0);
  EXPECT_THAT(actual, ElementsAre(0, 0, 0, 0, 0));
}

TEST(Type_test, ConfigItemValueT_equal) {
  const ConfigItemValueT item1 = {1, 0x11223344};
  const ConfigItemValueT item2 = item1;

  EXPECT_EQ(item1, item2);
}

TEST(Type_test, ConfigItemValueT_not_equal) {
  const ConfigItemValueT item1 = {1, 0x11223344};
  ConfigItemValueT item2 = item1;
  item2.configId++;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, ConfigItemValueT_toByteArray) {
  const ConfigItemValueT item1 = {1, 0x11223344};
  uint8_t actual[5]{};

  size_t size = item1.toByteArray(actual, sizeof(actual));

  EXPECT_GT(size, 0);
  EXPECT_THAT(actual, ElementsAre(1, 0x11, 0x22, 0x33, 0x44));
  EXPECT_EQ(size, 5);
}

TEST(Type_test, ConfigItemValueT_toByteArray_tooSmallBuf) {
  const ConfigItemValueT item1 = {1, 0x11223344};
  uint8_t actual[5]{};

  size_t size = item1.toByteArray(actual, sizeof(actual) - 1);

  EXPECT_EQ(size, 0);
  EXPECT_THAT(actual, ElementsAre(0, 0, 0, 0, 0));
}

TEST(Types_test, ConfigValuePayloadT_fromByteArray) {
  const uint8_t buf[] = {22,   2, 1,    0x11, 0x22, 0x33,
                         0x44, 2, 0x55, 0x66, 0x77, 0x88};
  ConfigValuePayloadT actual;

  EXPECT_EQ(actual.fromByteArray(buf, sizeof(buf)), 1);

  EXPECT_EQ(actual.entityId, 22);
  EXPECT_EQ(actual.numberOfConfigs, 2);
  EXPECT_EQ(actual.configValues[0].configId, 1);
  EXPECT_EQ(actual.configValues[0].value, 0x11223344);
  EXPECT_EQ(actual.configValues[1].configId, 2);
  EXPECT_EQ(actual.configValues[1].value, 0x55667788);
}

TEST(Types_test, ConfigValuePayloadT_fromByteArray_invalid_length) {
  const uint8_t buf[] = {22,   2, 1,    0x11, 0x22, 0x33,
                         0x44, 2, 0x55, 0x66, 0x77, 0x88};
  ConfigValuePayloadT actual;

  EXPECT_EQ(actual.fromByteArray(buf, sizeof(buf) - 1), 0);
}
