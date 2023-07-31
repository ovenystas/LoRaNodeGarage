#include "TemperatureSensor.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/BufferSerial.h"
#include "mocks/DHT.h"

using ::testing::Return;

using TemperatureT = int16_t;  // Degree C

BufferSerial Serial = BufferSerial(256);

class TemperatureSensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    pDhtMock = new DHTMock();
    pTs = new TemperatureSensor(15, "TemperatureSensor", *pDhtMock);
    strBuf[0] = '\0';
  }

  void TearDown() override {
    delete pTs;
    delete pDhtMock;
    releaseArduinoMock();
  }

  void bufSerReadStr() {
    size_t i = 0;
    while (Serial.available()) {
      int c = Serial.read();
      if (c < 0) {
        break;
      }
      strBuf[i++] = static_cast<char>(c);
    }
    strBuf[i] = '\0';
  }

  char strBuf[256];
  ArduinoMock *pArduinoMock;
  DHTMock *pDhtMock;
  TemperatureSensor *pTs;
};

TEST_F(TemperatureSensor_test, callService_shall_do_nothing) {
  pTs->callService(0);
}

TEST_F(TemperatureSensor_test, getConfigItemValues) {
  ConfigItemValueT items[4];

  EXPECT_EQ(pTs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);

  EXPECT_EQ(items[0].configId, 0);
  EXPECT_EQ(items[0].value, 10);

  EXPECT_EQ(items[1].configId, 1);
  EXPECT_EQ(items[1].value, 60);

  EXPECT_EQ(items[2].configId, 2);
  EXPECT_EQ(items[2].value, 60);

  EXPECT_EQ(items[3].configId, 3);
  EXPECT_EQ(items[3].value, 0);
}

TEST_F(TemperatureSensor_test, getDiscoveryItem) {
  DiscoveryItemT item;

  pTs->getDiscoveryItem(&item);

  EXPECT_EQ(item.entity.entityId, 15);
  EXPECT_EQ(item.entity.componentType,
            static_cast<uint8_t>(BaseComponent::Type::sensor));
  EXPECT_EQ(item.entity.deviceClass,
            static_cast<uint8_t>(SensorDeviceClass::temperature));
  EXPECT_EQ(item.entity.unit, static_cast<uint8_t>(Unit::Type::C));
  EXPECT_TRUE(item.entity.isSigned);
  EXPECT_EQ(item.entity.size, sizeof(TemperatureT) / 2);
  EXPECT_EQ(item.entity.precision, 1);

  EXPECT_EQ(item.numberOfConfigItems, 4);

  EXPECT_EQ(item.configItems[0].configId, 0);
  EXPECT_EQ(item.configItems[0].unit, static_cast<uint8_t>(Unit::Type::C));
  EXPECT_TRUE(item.configItems[0].isSigned);
  EXPECT_EQ(item.configItems[0].size, sizeof(TemperatureT) / 2);
  EXPECT_EQ(item.configItems[0].precision, 1);

  EXPECT_EQ(item.configItems[1].configId, 1);
  EXPECT_EQ(item.configItems[1].unit, static_cast<uint8_t>(Unit::Type::s));
  EXPECT_FALSE(item.configItems[1].isSigned);
  EXPECT_EQ(item.configItems[1].size, sizeof(uint16_t) / 2);
  EXPECT_EQ(item.configItems[1].precision, 0);

  EXPECT_EQ(item.configItems[2].configId, 2);
  EXPECT_EQ(item.configItems[2].unit, static_cast<uint8_t>(Unit::Type::s));
  EXPECT_FALSE(item.configItems[2].isSigned);
  EXPECT_EQ(item.configItems[2].size, sizeof(uint16_t) / 2);
  EXPECT_EQ(item.configItems[2].precision, 0);

  EXPECT_EQ(item.configItems[3].configId, 3);
  EXPECT_EQ(item.configItems[3].unit, static_cast<uint8_t>(Unit::Type::C));
  EXPECT_TRUE(item.configItems[3].isSigned);
  EXPECT_EQ(item.configItems[3].size, sizeof(TemperatureT) / 2);
  EXPECT_EQ(item.configItems[3].precision, 1);
}

TEST_F(TemperatureSensor_test, getEntityId) {
  EXPECT_EQ(pTs->getEntityId(), 15);
}

TEST_F(TemperatureSensor_test, getValueItem) {
  ValueItemT item;

  pTs->getValueItem(&item);

  EXPECT_EQ(item.entityId, 15);
  EXPECT_EQ(item.value, 0);
}

TEST_F(TemperatureSensor_test, print) {
  const char *expectStr = "TemperatureSensor: 0.0 °C";
  TemperatureSensor ts = TemperatureSensor(15, "TemperatureSensor", *pDhtMock);

  EXPECT_EQ(ts.print(Serial), strlen(expectStr));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
}

TEST_F(TemperatureSensor_test, print_service_shall_do_nothing) {
  TemperatureSensor ts = TemperatureSensor(15, "TemperatureSensor", *pDhtMock);
  EXPECT_EQ(ts.print(Serial, 0), 0);
}

TEST_F(TemperatureSensor_test, setConfigs_all_in_order) {
  ConfigItemValueT inItems[4] = {{0, 1000}, {1, 1001}, {2, 1002}, {3, 1003}};
  EXPECT_TRUE(pTs->setConfigItemValues(inItems, 4));

  ConfigItemValueT items[4];
  EXPECT_EQ(pTs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);
  EXPECT_EQ(items[0].value, 1000);
  EXPECT_EQ(items[1].value, 1001);
  EXPECT_EQ(items[2].value, 1002);
  EXPECT_EQ(items[3].value, 1003);
}

TEST_F(TemperatureSensor_test, setConfigs_all_out_of_order) {
  ConfigItemValueT inItems[4] = {{3, 2003}, {2, 2002}, {1, 2001}, {0, 2000}};
  EXPECT_TRUE(pTs->setConfigItemValues(inItems, 4));

  ConfigItemValueT items[4];
  EXPECT_EQ(pTs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);
  EXPECT_EQ(items[0].value, 2000);
  EXPECT_EQ(items[1].value, 2001);
  EXPECT_EQ(items[2].value, 2002);
  EXPECT_EQ(items[3].value, 2003);
}

TEST_F(TemperatureSensor_test, setConfigs_one) {
  ConfigItemValueT inItems[1] = {{3, 3003}};
  EXPECT_TRUE(pTs->setConfigItemValues(inItems, 1));

  ConfigItemValueT items[4];
  EXPECT_EQ(pTs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);
  EXPECT_EQ(items[0].value, 10);
  EXPECT_EQ(items[1].value, 60);
  EXPECT_EQ(items[2].value, 60);
  EXPECT_EQ(items[3].value, 3003);
}

TEST_F(TemperatureSensor_test, setConfigs_too_many) {
  ConfigItemValueT inItems[5] = {
      {0, 1000}, {1, 1001}, {2, 1002}, {3, 1003}, {4, 1004}};
  EXPECT_FALSE(pTs->setConfigItemValues(inItems, 5));
}

TEST_F(TemperatureSensor_test, setConfigs_out_of_range) {
  ConfigItemValueT inItems[1] = {{4, 3004}};
  EXPECT_FALSE(pTs->setConfigItemValues(inItems, 1));
}

TEST_F(TemperatureSensor_test,
       update_largeValueDiff_largeTimeDiff_withConfigsZero_shall_return_false) {
  EXPECT_CALL(*pDhtMock, read(false)).WillOnce(Return(true));
  EXPECT_CALL(*pDhtMock, readTemperature(false, false)).WillOnce(Return(0.950));
  EXPECT_CALL(*pArduinoMock, millis()).Times(0);
  ConfigItemValueT inItems[4] = {{0, 0}, {1, 0}, {2, 0}, {3, 0}};
  EXPECT_TRUE(pTs->setConfigItemValues(inItems, 4));
  EXPECT_FALSE(pTs->update());
}

TEST_F(TemperatureSensor_test, setReported) {
  EXPECT_CALL(*pArduinoMock, millis()).Times(1);
  pTs->setReported();
}

TEST_F(TemperatureSensor_test,
       update_smallValueDiff_smallTimeDiff_shall_return_false) {
  EXPECT_CALL(*pDhtMock, read(false)).WillOnce(Return(true));
  EXPECT_CALL(*pDhtMock, readTemperature(false, false)).WillOnce(Return(0.949));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_FALSE(pTs->update());
}

TEST_F(TemperatureSensor_test,
       update_smallValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDhtMock, read(false)).WillOnce(Return(true));
  EXPECT_CALL(*pDhtMock, readTemperature(false, false)).WillOnce(Return(0.949));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pTs->update());
}

TEST_F(TemperatureSensor_test,
       update_largeValueDiff_smallTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDhtMock, read(false)).WillOnce(Return(true));
  EXPECT_CALL(*pDhtMock, readTemperature(false, false)).WillOnce(Return(0.950));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_TRUE(pTs->update());
}

TEST_F(TemperatureSensor_test,
       update_largeValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pDhtMock, read(false)).WillOnce(Return(true));
  EXPECT_CALL(*pDhtMock, readTemperature(false, false)).WillOnce(Return(0.950));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pTs->update());
}
