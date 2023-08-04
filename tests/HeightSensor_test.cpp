#include "HeightSensor.h"

#include <gtest/gtest.h>

#include "Sensor.h"
#include "Unit.h"
#include "mocks/BufferSerial.h"
#include "mocks/EEPROM.h"

using ::testing::Return;

using HeightT = int16_t;  // cm

class HeightSensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pSerial = new BufferSerial(256);
    pArduinoMock = arduinoMockInstance();
    eeprom_clear();
    strBuf[0] = '\0';

    pDistanceSensor = new Sensor<DistanceT>(
        99, "Distance", SensorDeviceClass::distance, Unit::Type::cm);
    pHs = new HeightSensor(39, "HeightSensor", *pDistanceSensor);
  }

  void TearDown() override {
    delete pHs;
    delete pDistanceSensor;
    delete pSerial;
    releaseArduinoMock();
  }

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
  ArduinoMock* pArduinoMock;
  Sensor<DistanceT>* pDistanceSensor;
  HeightSensor* pHs;
};

TEST_F(HeightSensor_test, callService_shall_do_nothing) { pHs->callService(0); }

TEST_F(HeightSensor_test, getConfigItemValues) {
  ConfigItemValueT items[4];

  EXPECT_EQ(pHs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);

  EXPECT_EQ(items[0].configId, 0);
  EXPECT_EQ(items[0].value, 10);

  EXPECT_EQ(items[1].configId, 1);
  EXPECT_EQ(items[1].value, 60);

  EXPECT_EQ(items[2].configId, 2);
  EXPECT_EQ(items[2].value, 5000);

  EXPECT_EQ(items[3].configId, 3);
  EXPECT_EQ(items[3].value, 60);
}

TEST_F(HeightSensor_test, getDiscoveryItem) {
  DiscoveryItemT item;

  pHs->getDiscoveryItem(&item);

  EXPECT_EQ(item.entity.entityId, 39);
  EXPECT_EQ(item.entity.componentType,
            static_cast<uint8_t>(BaseComponent::Type::sensor));
  EXPECT_EQ(item.entity.deviceClass,
            static_cast<uint8_t>(SensorDeviceClass::distance));
  EXPECT_EQ(item.entity.unit, static_cast<uint8_t>(Unit::Type::cm));
  EXPECT_TRUE(item.entity.isSigned);
  EXPECT_EQ(item.entity.sizeCode, sizeof(HeightT) / 2);
  EXPECT_EQ(item.entity.precision, 0);

  EXPECT_EQ(item.numberOfConfigItems, 4);

  EXPECT_EQ(item.configItems[0].configId, 0);
  EXPECT_EQ(item.configItems[0].unit, static_cast<uint8_t>(Unit::Type::cm));
  EXPECT_TRUE(item.configItems[0].isSigned);
  EXPECT_EQ(item.configItems[0].sizeCode, sizeof(HeightT) / 2);
  EXPECT_EQ(item.configItems[0].precision, 0);

  EXPECT_EQ(item.configItems[1].configId, 1);
  EXPECT_EQ(item.configItems[1].unit, static_cast<uint8_t>(Unit::Type::s));
  EXPECT_FALSE(item.configItems[1].isSigned);
  EXPECT_EQ(item.configItems[1].sizeCode, sizeof(uint16_t) / 2);
  EXPECT_EQ(item.configItems[1].precision, 0);

  EXPECT_EQ(item.configItems[2].configId, 2);
  EXPECT_EQ(item.configItems[2].unit, static_cast<uint8_t>(Unit::Type::ms));
  EXPECT_FALSE(item.configItems[2].isSigned);
  EXPECT_EQ(item.configItems[2].sizeCode, sizeof(uint16_t) / 2);
  EXPECT_EQ(item.configItems[2].precision, 0);

  EXPECT_EQ(item.configItems[3].configId, 3);
  EXPECT_EQ(item.configItems[3].unit, static_cast<uint8_t>(Unit::Type::cm));
  EXPECT_TRUE(item.configItems[3].isSigned);
  EXPECT_EQ(item.configItems[3].sizeCode, sizeof(HeightT) / 2);
  EXPECT_EQ(item.configItems[3].precision, 0);
}

TEST_F(HeightSensor_test, getEntityId) { EXPECT_EQ(pHs->getEntityId(), 39); }

TEST_F(HeightSensor_test, getSensor) {
  Sensor<HeightT>& sensor = pHs->getSensor();
  EXPECT_EQ(sensor.getEntityId(), 39);
}

TEST_F(HeightSensor_test, getValueItem) {
  ValueItemT item;

  pHs->getValueItem(&item);

  EXPECT_EQ(item.entityId, 39);
  EXPECT_EQ(item.value, 0);
}

TEST_F(HeightSensor_test, print) {
  const char* expectStr = "HeightSensor: 0 cm";
  Sensor<DistanceT> ds = Sensor<DistanceT>(
      99, "Distance", SensorDeviceClass::distance, Unit::Type::cm);
  HeightSensor hs = HeightSensor(39, "HeightSensor", ds);

  EXPECT_EQ(hs.print(*pSerial), 18);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
}

TEST_F(HeightSensor_test, print_service_shall_do_nothing) {
  Sensor<DistanceT> ds = Sensor<DistanceT>(
      99, "Distance", SensorDeviceClass::distance, Unit::Type::cm);
  HeightSensor hs = HeightSensor(39, "HeightSensor", ds);
  EXPECT_EQ(hs.print(*pSerial, 0), 0);
}

TEST_F(HeightSensor_test, setConfigs_all_in_order) {
  ConfigItemValueT inItems[4] = {{0, 1000}, {1, 1001}, {2, 1002}, {3, 1003}};
  EXPECT_TRUE(pHs->setConfigItemValues(inItems, 4));

  ConfigItemValueT items[4];
  EXPECT_EQ(pHs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);

  EXPECT_EQ(items[0].value, 1000);
  EXPECT_EQ(items[1].value, 1001);
  EXPECT_EQ(items[2].value, 1002);
  EXPECT_EQ(items[3].value, 1003);
}

TEST_F(HeightSensor_test, setConfigs_all_out_of_order) {
  ConfigItemValueT inItems[4] = {{3, 2003}, {2, 2002}, {1, 2001}, {0, 2000}};
  EXPECT_TRUE(pHs->setConfigItemValues(inItems, 4));

  ConfigItemValueT items[4];
  EXPECT_EQ(pHs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);
  EXPECT_EQ(items[0].value, 2000);
  EXPECT_EQ(items[1].value, 2001);
  EXPECT_EQ(items[2].value, 2002);
  EXPECT_EQ(items[3].value, 2003);
}

TEST_F(HeightSensor_test, setConfigs_one) {
  ConfigItemValueT inItems[1] = {{3, 3003}};
  EXPECT_TRUE(pHs->setConfigItemValues(inItems, 1));

  ConfigItemValueT items[4];
  EXPECT_EQ(pHs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);
  EXPECT_EQ(items[0].value, 10);
  EXPECT_EQ(items[1].value, 60);
  EXPECT_EQ(items[2].value, 5000);
  EXPECT_EQ(items[3].value, 3003);
}

TEST_F(HeightSensor_test, setConfigs_too_many) {
  ConfigItemValueT inItems[5] = {
      {0, 1000}, {1, 1001}, {2, 1002}, {3, 1003}, {4, 1004}};
  EXPECT_FALSE(pHs->setConfigItemValues(inItems, 5));
}

TEST_F(HeightSensor_test, setConfigs_out_of_range) {
  ConfigItemValueT inItems[1] = {{4, 3004}};
  EXPECT_FALSE(pHs->setConfigItemValues(inItems, 1));
}

TEST_F(HeightSensor_test, setReported) {
  EXPECT_CALL(*pArduinoMock, millis()).Times(1);
  pHs->setReported();
}

TEST_F(HeightSensor_test,
       update_largeValueDiff_largeTimeDiff_withConfigsZero_shall_return_false) {
  pDistanceSensor->setValue(70);
  EXPECT_CALL(*pArduinoMock, millis()).Times(0);
  ConfigItemValueT inItems[4] = {{0, 0}, {1, 0}, {2, 0}, {3, 0}};
  EXPECT_TRUE(pHs->setConfigItemValues(inItems, 4));
  EXPECT_FALSE(pHs->update());
}

TEST_F(HeightSensor_test,
       update_smallValueDiff_smallTimeDiff_shall_return_false) {
  pDistanceSensor->setValue(69);
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_FALSE(pHs->update());
}

TEST_F(HeightSensor_test,
       update_smallValueDiff_largeTimeDiff_shall_return_true) {
  pDistanceSensor->setValue(69);
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pHs->update());
}

TEST_F(HeightSensor_test,
       update_largeValueDiff_smallTimeDiff_shall_return_true) {
  pDistanceSensor->setValue(70);
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(59999));
  EXPECT_TRUE(pHs->update());
}

TEST_F(HeightSensor_test,
       update_largeValueDiff_largeTimeDiff_shall_return_true) {
  pDistanceSensor->setValue(70);
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(60000));
  EXPECT_TRUE(pHs->update());
}
