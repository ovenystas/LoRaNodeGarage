#include "DistanceSensor.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"
#include "mocks/EEPROM.h"
#include "mocks/NewPing.h"

#define SONAR_TRIGGER_PIN 7
#define SONAR_ECHO_PIN 6
#define SONAR_MAX_DISTANCE_CM 300

using ::testing::Return;

using DistanceT = int16_t;  // cm

class DistanceSensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    pSonarMock = new NewPingMock();
    strBuf[0] = '\0';
    eeprom_clear();

    pDs = new DistanceSensor(7, "DistanceSensor", *pSonarMock);
  }

  void TearDown() override {
    delete pDs;
    delete pSonarMock;
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
  ArduinoMock* pArduinoMock;
  NewPingMock* pSonarMock;
  DistanceSensor* pDs;
};

TEST_F(DistanceSensor_test, callService_shall_do_nothing) {
  pDs->callService(0);
}

TEST_F(DistanceSensor_test, getConfigItemValues) {
  ConfigItemValueT items[3];

  EXPECT_EQ(pDs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            3);

  EXPECT_EQ(items[0].configId, 0);
  EXPECT_EQ(items[0].value,
            DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT);

  EXPECT_EQ(items[1].configId, 1);
  EXPECT_EQ(items[1].value,
            DistanceSensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT);

  EXPECT_EQ(items[2].configId, 2);
  EXPECT_EQ(items[2].value,
            DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT);
}

TEST_F(DistanceSensor_test, getDiscoveryItem) {
  DiscoveryItemT item;

  pDs->getDiscoveryItem(&item);

  EXPECT_EQ(item.entity.entityId, 7);
  EXPECT_EQ(item.entity.componentType,
            static_cast<uint8_t>(BaseComponent::Type::sensor));
  EXPECT_EQ(item.entity.deviceClass,
            static_cast<uint8_t>(SensorDeviceClass::distance));
  EXPECT_EQ(item.entity.unit, static_cast<uint8_t>(Unit::Type::cm));
  EXPECT_TRUE(item.entity.isSigned);
  EXPECT_EQ(item.entity.sizeCode, sizeof(DistanceT) / 2);
  EXPECT_EQ(item.entity.precision, 0);

  EXPECT_EQ(item.numberOfConfigItems, 3);

  EXPECT_EQ(item.configItems[0].configId, 0);
  EXPECT_EQ(item.configItems[0].unit, static_cast<uint8_t>(Unit::Type::cm));
  EXPECT_TRUE(item.configItems[0].isSigned);
  EXPECT_EQ(item.configItems[0].sizeCode, sizeof(DistanceT) / 2);
  EXPECT_EQ(item.configItems[0].precision, 0);

  EXPECT_EQ(item.configItems[1].configId, 1);
  EXPECT_EQ(item.configItems[1].unit, static_cast<uint8_t>(Unit::Type::s));
  EXPECT_FALSE(item.configItems[1].isSigned);
  EXPECT_EQ(item.configItems[1].sizeCode, sizeof(uint16_t) / 2);
  EXPECT_EQ(item.configItems[1].precision, 0);

  EXPECT_EQ(item.configItems[2].configId, 2);
  EXPECT_EQ(item.configItems[2].unit, static_cast<uint8_t>(Unit::Type::s));
  EXPECT_FALSE(item.configItems[2].isSigned);
  EXPECT_EQ(item.configItems[2].sizeCode, sizeof(uint16_t) / 2);
  EXPECT_EQ(item.configItems[2].precision, 0);
}

TEST_F(DistanceSensor_test, getEntityId) { EXPECT_EQ(pDs->getEntityId(), 7); }

TEST_F(DistanceSensor_test, getSensor) {
  Sensor<DistanceT>& sensor = pDs->getSensor();
  EXPECT_EQ(sensor.getEntityId(), 7);
}

TEST_F(DistanceSensor_test, getValueItem) {
  ValueItemT item;

  pDs->getValueItem(&item);

  EXPECT_EQ(item.entityId, 7);
  EXPECT_EQ(item.value, 0);
}

TEST_F(DistanceSensor_test,
       loadConfigValues_wrong_crc_shall_set_default_values) {
  pDs->loadConfigValues();

  ConfigItemValueT items[3];
  EXPECT_EQ(pDs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            3);
  EXPECT_EQ(items[0].value,
            DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT);
  EXPECT_EQ(items[1].value,
            DistanceSensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT);
  EXPECT_EQ(items[2].value,
            DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT);
}

TEST_F(DistanceSensor_test, loadConfigValues_OK) {
  (void)EEPROM.put(
      EE_ADDRESS_CONFIG_DISTANCESENSOR_0,
      DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT + 1);
  eeprom_write_byte(EE_ADDRESS_CONFIG_DISTANCESENSOR_0 + sizeof(DistanceT),
                    0x31);  // Correct CRC8 of value
  (void)EEPROM.put(
      EE_ADDRESS_CONFIG_DISTANCESENSOR_1,
      DistanceSensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT + 1);
  eeprom_write_byte(EE_ADDRESS_CONFIG_DISTANCESENSOR_1 + sizeof(DistanceT),
                    0xB3);  // Correct CRC8 of value
  (void)EEPROM.put(EE_ADDRESS_CONFIG_DISTANCESENSOR_2,
                   DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT + 1);
  eeprom_write_byte(EE_ADDRESS_CONFIG_DISTANCESENSOR_2 + sizeof(DistanceT),
                    0xD6);  // Correct CRC8 of value

  pDs->loadConfigValues();

  ConfigItemValueT items[3];
  EXPECT_EQ(pDs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            3);
  EXPECT_EQ(items[0].value,
            DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT + 1);
  EXPECT_EQ(items[1].value,
            DistanceSensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT + 1);
  EXPECT_EQ(items[2].value,
            DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT + 1);
}

TEST_F(DistanceSensor_test, print) {
  const char* expectStr = "DistanceSensor: 0 cm";
  NewPingMock sonarMock;
  DistanceSensor ds = DistanceSensor(7, "DistanceSensor", sonarMock);

  EXPECT_EQ(ds.printTo(Serial), strlen(expectStr));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
}

TEST_F(DistanceSensor_test, print_service_shall_do_nothing) {
  NewPingMock sonarMock;
  DistanceSensor ds = DistanceSensor(7, "DistanceSensor", sonarMock);
  EXPECT_EQ(ds.printTo(Serial, 0), 0);
}

TEST_F(DistanceSensor_test, setConfigs_all_in_order) {
  ConfigItemValueT inItems[3] = {{0, 1000}, {1, 1001}, {2, 1002}};

  EXPECT_TRUE(pDs->setConfigItemValues(inItems, 3));

  ConfigItemValueT items[3];
  EXPECT_EQ(pDs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            3);
  EXPECT_EQ(items[0].value, 1000);
  EXPECT_EQ(items[1].value, 1001);
  EXPECT_EQ(items[2].value, 1002);
}

TEST_F(DistanceSensor_test, setConfigs_all_out_of_order) {
  ConfigItemValueT inItems[3] = {{2, 2002}, {1, 2001}, {0, 2000}};

  EXPECT_TRUE(pDs->setConfigItemValues(inItems, 3));

  ConfigItemValueT items[3];
  EXPECT_EQ(pDs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            3);
  EXPECT_EQ(items[0].value, 2000);
  EXPECT_EQ(items[1].value, 2001);
  EXPECT_EQ(items[2].value, 2002);
}

TEST_F(DistanceSensor_test, setConfigs_one) {
  ConfigItemValueT inItems[1] = {{2, 3002}};

  EXPECT_TRUE(pDs->setConfigItemValues(inItems, 1));

  ConfigItemValueT items[3];
  EXPECT_EQ(pDs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            3);
  EXPECT_EQ(items[0].value,
            DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT);
  EXPECT_EQ(items[1].value,
            DistanceSensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT);
  EXPECT_EQ(items[2].value, 3002);
}

TEST_F(DistanceSensor_test, setConfigs_too_many) {
  ConfigItemValueT inItems[4] = {{0, 1000}, {1, 1001}, {2, 1002}, {3, 1003}};

  EXPECT_FALSE(pDs->setConfigItemValues(inItems, 4));
}

TEST_F(DistanceSensor_test, setConfigs_out_of_range) {
  ConfigItemValueT inItems[1] = {{4, 3004}};

  EXPECT_FALSE(pDs->setConfigItemValues(inItems, 1));
}

TEST_F(DistanceSensor_test, setReported) {
  EXPECT_CALL(*pArduinoMock, millis()).Times(1);

  pDs->setReported();
}

TEST_F(DistanceSensor_test,
       update_largeValueDiff_largeTimeDiff_withConfigsZero_shall_return_false) {
  EXPECT_CALL(*pSonarMock, ping_cm(0))
      .WillOnce(
          Return(DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT));
  EXPECT_CALL(*pArduinoMock, millis()).Times(0);

  ConfigItemValueT inItems[3] = {{0, 0}, {1, 0}, {2, 0}};
  EXPECT_TRUE(pDs->setConfigItemValues(inItems, 3));

  EXPECT_FALSE(pDs->update());
}

TEST_F(DistanceSensor_test,
       update_smallValueDiff_smallTimeDiff_shall_return_false) {
  EXPECT_CALL(*pSonarMock, ping_cm(0))
      .WillOnce(Return(
          DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT - 1));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(
          DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT * 1000 - 1));

  EXPECT_FALSE(pDs->update());
}

TEST_F(DistanceSensor_test,
       update_smallValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pSonarMock, ping_cm(0))
      .WillOnce(Return(
          DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT - 1));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT *
                       1000));

  EXPECT_TRUE(pDs->update());
}

TEST_F(DistanceSensor_test,
       update_largeValueDiff_smallTimeDiff_shall_return_true) {
  EXPECT_CALL(*pSonarMock, ping_cm(0))
      .WillOnce(
          Return(DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(
          Return(DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT - 1));

  EXPECT_TRUE(pDs->update());
}

TEST_F(DistanceSensor_test,
       update_largeValueDiff_largeTimeDiff_shall_return_true) {
  EXPECT_CALL(*pSonarMock, ping_cm(0))
      .WillOnce(Return(
          DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT - 1));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT *
                       1000));

  EXPECT_TRUE(pDs->update());
}

TEST_F(DistanceSensor_test, isReportDue) {
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(0));

  // Newly constructed shall be true
  EXPECT_TRUE(pDs->isReportDue());

  // Shall be set to false when setReported() is called
  pDs->setReported();
  EXPECT_FALSE(pDs->isReportDue());
}
