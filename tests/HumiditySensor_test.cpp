#include "HumiditySensor.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/BufferSerial.h"
#include "mocks/DHT.h"

using ::testing::Return;

using HumidityT = int8_t;  // cm

class HumiditySensorPrint_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pSerial = new BufferSerial(256);
    strBuf[0] = '\0';
  }

  void TearDown() override { delete pSerial; }

  void bufSerReadStr() {
    size_t i = 0;
    while (pSerial->available()) {
      int c = pSerial->read();
      if (c < 0) {
        break;
      }
      strBuf[i++] = static_cast<char>(c);
    }
    strBuf[i] = '\0';
  }

  char strBuf[256];
  BufferSerial* pSerial;
};

class HumiditySensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    pDhtMock = new DHTMock();
    pHs = new HumiditySensor(51, "HumiditySensor", *pDhtMock);
  }

  void TearDown() override {
    delete pHs;
    delete pDhtMock;
    releaseArduinoMock();
  }

  ArduinoMock* pArduinoMock;
  DHTMock* pDhtMock;
  HumiditySensor* pHs;
};

TEST_F(HumiditySensor_test, callService_shall_do_nothing) {
  pHs->callService(0);
}

TEST_F(HumiditySensor_test, getConfigItemValues) {
  ConfigItemValueT items[4];

  EXPECT_EQ(pHs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);

  EXPECT_EQ(items[0].configId, 0);
  EXPECT_EQ(items[0].value, 2);

  EXPECT_EQ(items[1].configId, 1);
  EXPECT_EQ(items[1].value, 60);

  EXPECT_EQ(items[2].configId, 2);
  EXPECT_EQ(items[2].value, 60);

  EXPECT_EQ(items[3].configId, 3);
  EXPECT_EQ(items[3].value, 0);
}

TEST_F(HumiditySensor_test, getDiscoveryItem) {
  DiscoveryItemT item;

  pHs->getDiscoveryItem(&item);

  EXPECT_EQ(item.entity.entityId, 51);
  EXPECT_EQ(item.entity.componentType,
            static_cast<uint8_t>(BaseComponent::Type::sensor));
  EXPECT_EQ(item.entity.deviceClass,
            static_cast<uint8_t>(SensorDeviceClass::humidity));
  EXPECT_EQ(item.entity.unit, static_cast<uint8_t>(Unit::Type::percent));
  EXPECT_EQ(item.entity.size, sizeof(HumidityT));
  EXPECT_EQ(item.entity.precision, 0);

  EXPECT_EQ(item.numberOfConfigItems, 4);

  EXPECT_EQ(item.configItems[0].configId, 0);
  EXPECT_EQ(item.configItems[0].unit,
            static_cast<uint8_t>(Unit::Type::percent));
  EXPECT_EQ(item.configItems[0].size, sizeof(HumidityT));
  EXPECT_EQ(item.configItems[0].precision, 0);

  EXPECT_EQ(item.configItems[1].configId, 1);
  EXPECT_EQ(item.configItems[1].unit, static_cast<uint8_t>(Unit::Type::s));
  EXPECT_EQ(item.configItems[1].size, sizeof(uint16_t));
  EXPECT_EQ(item.configItems[1].precision, 0);

  EXPECT_EQ(item.configItems[2].configId, 2);
  EXPECT_EQ(item.configItems[2].unit, static_cast<uint8_t>(Unit::Type::s));
  EXPECT_EQ(item.configItems[2].size, sizeof(uint16_t));
  EXPECT_EQ(item.configItems[2].precision, 0);

  EXPECT_EQ(item.configItems[3].configId, 3);
  EXPECT_EQ(item.configItems[3].unit,
            static_cast<uint8_t>(Unit::Type::percent));
  EXPECT_EQ(item.configItems[3].size, sizeof(HumidityT));
  EXPECT_EQ(item.configItems[3].precision, 0);
}

TEST_F(HumiditySensor_test, getEntityId) { EXPECT_EQ(pHs->getEntityId(), 51); }

TEST_F(HumiditySensor_test, getValueItem) {
  ValueItemT item;

  pHs->getValueItem(&item);

  EXPECT_EQ(item.entityId, 51);
  EXPECT_EQ(item.value, 0);
}

TEST_F(HumiditySensorPrint_test, print) {
  const char* expectStr = "HumiditySensor: 0 %";
  DHTMock* pDhtMock = new DHTMock();
  HumiditySensor hs = HumiditySensor(51, "HumiditySensor", *pDhtMock);

  EXPECT_EQ(hs.print(*pSerial), strlen(expectStr));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
}

TEST_F(HumiditySensorPrint_test, print_service_shall_do_nothing) {
  DHTMock* pDhtMock = new DHTMock();
  HumiditySensor hs = HumiditySensor(51, "HumiditySensor", *pDhtMock);
  EXPECT_EQ(hs.print(*pSerial, 0), 0);
}

TEST_F(HumiditySensor_test, setConfigs_all_in_order) {
  ConfigItemValueT inItems[4] = {{0, 100}, {1, 1001}, {2, 1002}, {3, 103}};
  EXPECT_TRUE(pHs->setConfigItemValues(inItems, 4));

  ConfigItemValueT items[4];
  EXPECT_EQ(pHs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);
  EXPECT_EQ(items[0].value, 100);
  EXPECT_EQ(items[1].value, 1001);
  EXPECT_EQ(items[2].value, 1002);
  EXPECT_EQ(items[3].value, 103);
}

TEST_F(HumiditySensor_test, setConfigs_all_out_of_order) {
  ConfigItemValueT inItems[4] = {{3, 23}, {2, 2002}, {1, 2001}, {0, 20}};
  EXPECT_TRUE(pHs->setConfigItemValues(inItems, 4));

  ConfigItemValueT items[4];
  EXPECT_EQ(pHs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);
  EXPECT_EQ(items[0].value, 20);
  EXPECT_EQ(items[1].value, 2001);
  EXPECT_EQ(items[2].value, 2002);
  EXPECT_EQ(items[3].value, 23);
}

TEST_F(HumiditySensor_test, setConfigs_one) {
  ConfigItemValueT inItems[1] = {{3, 33}};
  EXPECT_TRUE(pHs->setConfigItemValues(inItems, 1));

  ConfigItemValueT items[4];
  EXPECT_EQ(pHs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);
  EXPECT_EQ(items[0].value, 2);
  EXPECT_EQ(items[1].value, 60);
  EXPECT_EQ(items[2].value, 60);
  EXPECT_EQ(items[3].value, 33);
}

TEST_F(HumiditySensor_test, setConfigs_too_many) {
  ConfigItemValueT inItems[5] = {
      {0, 100}, {1, 1001}, {2, 1002}, {3, 103}, {4, 1004}};
  EXPECT_FALSE(pHs->setConfigItemValues(inItems, 5));
}

TEST_F(HumiditySensor_test, setConfigs_out_of_range) {
  ConfigItemValueT inItems[1] = {{4, 3004}};
  EXPECT_FALSE(pHs->setConfigItemValues(inItems, 1));
}

TEST_F(HumiditySensor_test, setReported) {
  EXPECT_CALL(*pArduinoMock, millis()).Times(1);
  pHs->setReported();
}

TEST_F(HumiditySensor_test,
       update_largeValueDiff_largeTimeDiff_withConfigsZero_shall_return_false) {
  EXPECT_CALL(*pDhtMock, readHumidity(false)).WillOnce(Return(2));
  EXPECT_CALL(*pArduinoMock, millis()).Times(0);
  ConfigItemValueT inItems[4] = {{0, 0}, {1, 0}, {2, 0}, {3, 0}};
  EXPECT_TRUE(pHs->setConfigItemValues(inItems, 4));
  EXPECT_FALSE(pHs->update());
}

TEST_F(HumiditySensor_test,
       update_smallValueDiff_smallTimeDiff_shall_return_false) {
  EXPECT_CALL(*pDhtMock, readHumidity(false)).WillOnce(Return(1));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_FALSE(pHs->update());
}

TEST_F(HumiditySensor_test,
       update_smallValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDhtMock, readHumidity(false)).WillOnce(Return(1));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pHs->update());
}

TEST_F(HumiditySensor_test,
       update_largeValueDiff_smallTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDhtMock, readHumidity(false)).WillOnce(Return(2));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_TRUE(pHs->update());
}

TEST_F(HumiditySensor_test,
       update_largeValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDhtMock, readHumidity(false)).WillOnce(Return(2));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pHs->update());
}
