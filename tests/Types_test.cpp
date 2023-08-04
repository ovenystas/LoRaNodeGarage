#include "Types.h"

#include <gtest/gtest.h>

using ::testing::ElementsAre;

TEST(Type_test, DiscoveryEntityItemT_equal) {
  const DiscoveryEntityItemT item1 = {1, 2, 3, 4, 3, 2, 1, 0};
  const DiscoveryEntityItemT item2 = item1;

  EXPECT_EQ(item1, item2);
}

TEST(Type_test, DiscoveryEntityItemT_not_equal) {
  const DiscoveryEntityItemT item1 = {1, 2, 3, 4, 3, 2, 1, 0};
  DiscoveryEntityItemT item2 = item1;
  item2.precision++;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, DiscoveryEntityItemT_toByteArray) {
  const DiscoveryEntityItemT item1 = {1, 2, 3, 4, 3, 2, 1, 0};
  uint8_t actual[5]{};

  EXPECT_EQ(item1.toByteArray(actual, sizeof(actual)), 5);

  EXPECT_THAT(actual, ElementsAre(1, 2, 3, 4, 0x1B));
}

TEST(Type_test, DiscoveryEntityItemT_toByteArray_tooSmallBuf) {
  const DiscoveryEntityItemT item1 = {1, 2, 3, 4, 3, 2, 1, 0};
  uint8_t actual[5]{};

  EXPECT_EQ(item1.toByteArray(actual, sizeof(actual) - 1), 0);

  EXPECT_THAT(actual, ElementsAre(0, 0, 0, 0, 0));
}

TEST(Type_test, DiscoveryConfigItemT_equal) {
  const DiscoveryConfigItemT item1 = {1, 2, 3, 2, 1, 0};
  const DiscoveryConfigItemT item2 = item1;

  EXPECT_EQ(item1, item2);
}

TEST(Type_test, DiscoveryConfigItemT_not_equal) {
  const DiscoveryConfigItemT item1 = {1, 2, 3, 2, 1, 0};
  DiscoveryConfigItemT item2 = item1;
  item2.precision++;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, DiscoveryConfigItemT_toByteArray) {
  const DiscoveryConfigItemT item1 = {1, 2, 3, 2, 1, 0};
  uint8_t actual[3]{};

  EXPECT_EQ(item1.toByteArray(actual, sizeof(actual)), 3);

  EXPECT_THAT(actual, ElementsAre(1, 2, 0x1B));
}

TEST(Type_test, DiscoveryConfigItemT_toByteArray_tooSmallBuf) {
  const DiscoveryConfigItemT item1 = {1, 2, 3, 2, 1, 0};
  uint8_t actual[3]{};

  EXPECT_EQ(item1.toByteArray(actual, sizeof(actual) - 1), 0);

  EXPECT_THAT(actual, ElementsAre(0, 0, 0));
}

TEST(Type_test, DiscoveryItemT_equal) {
  const DiscoveryItemT item1 = {
      1, 2, 3, 4, 3, 2, 1, 0, 2, {{0, 1, 3, 2, 1, 0}, {1, 4, 3, 2, 1, 0}}};
  const DiscoveryItemT item2 = item1;

  EXPECT_EQ(item1, item2);
}

TEST(Type_test, DiscoveryItemT_equal_ignore_configItems_outOfRange) {
  const DiscoveryItemT item1 = {
      1, 2, 3, 4, 3, 2, 1, 0, 1, {{0, 1, 3, 2, 1, 0}, {1, 4, 3, 2, 1, 0}}};
  DiscoveryItemT item2 = item1;
  item2.configItems[1].precision++;

  EXPECT_EQ(item1, item2);
}

TEST(Type_test, DiscoveryItemT_entity_not_equal) {
  const DiscoveryItemT item1 = {
      1, 2, 3, 4, 3, 2, 1, 0, 2, {{0, 1, 3, 2, 1, 0}, {1, 4, 3, 2, 1, 0}}};
  DiscoveryItemT item2 = item1;
  item2.entity.sizeCode++;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, DiscoveryItemT_numberOfConfigItems_not_equal) {
  const DiscoveryItemT item1 = {
      1, 2, 3, 4, 3, 2, 1, 0, 2, {{0, 1, 3, 2, 1, 0}, {1, 4, 3, 2, 1, 0}}};
  DiscoveryItemT item2 = item1;
  item2.numberOfConfigItems++;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, DiscoveryItemT_numberOfConfigItems_tooLarge) {
  const DiscoveryItemT item1 = {
      1, 2, 3, 4, 3, 2, 1, 0, 14, {{0, 1, 3, 2, 1, 0}, {1, 4, 3, 2, 1, 0}}};
  DiscoveryItemT item2 = item1;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, DiscoveryItemT_configItems_not_equal) {
  const DiscoveryItemT item1 = {
      1, 2, 3, 4, 3, 2, 1, 0, 13, {{0, 1, 3, 2, 1, 0}, {1, 4, 3, 2, 1, 0}}};
  DiscoveryItemT item2 = item1;
  item2.configItems[12].sizeCode++;

  EXPECT_NE(item1, item2);
}

TEST(Type_test, DiscoveryItemT_toByteArray) {
  const DiscoveryItemT item1 = {
      1, 2, 3, 4, 3, 2, 1, 0, 2, {{0, 1, 3, 2, 1, 0}, {1, 4, 3, 2, 1, 0}}};
  uint8_t actual[12]{};

  EXPECT_EQ(item1.toByteArray(actual, sizeof(actual)), 12);

  EXPECT_THAT(actual, ElementsAre(1, 2, 3, 4, 0x1B, 2, 0, 1, 0x1B, 1, 4, 0x1B));
}

TEST(Type_test, DiscoveryItemT_toByteArray_tooSmallBuf) {
  const DiscoveryItemT item1 = {
      1, 2, 3, 4, 3, 2, 1, 0, 2, {{0, 1, 3, 2, 1, 0}, {1, 4, 3, 2, 1, 0}}};
  uint8_t actual[12]{};

  EXPECT_EQ(item1.toByteArray(actual, sizeof(actual) - 1), 0);

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

  EXPECT_EQ(item1.toByteArray(actual, sizeof(actual)), 5);

  EXPECT_THAT(actual, ElementsAre(1, 0x11, 0x22, 0x33, 0x44));
}

TEST(Type_test, ValueItemT_toByteArray_tooSmallBuf) {
  const ValueItemT item1 = {1, 0x11223344};
  uint8_t actual[5]{};

  EXPECT_EQ(item1.toByteArray(actual, sizeof(actual) - 1), 0);

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

  EXPECT_EQ(item1.toByteArray(actual, sizeof(actual)), 5);

  EXPECT_THAT(actual, ElementsAre(1, 0x11, 0x22, 0x33, 0x44));
}

TEST(Type_test, ConfigItemValueT_toByteArray_tooSmallBuf) {
  const ConfigItemValueT item1 = {1, 0x11223344};
  uint8_t actual[5]{};

  EXPECT_EQ(item1.toByteArray(actual, sizeof(actual) - 1), 0);

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
