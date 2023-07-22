#include "PresenceBinarySensor.h"

#include <gtest/gtest.h>

#include "Sensor.h"
#include "Unit.h"
#include "mocks/BufferSerial.h"

using ::testing::Return;

using HeightT = int16_t;  // cm

class PresenceBinarySensorPrint_test : public ::testing::Test {
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

class PresenceBinarySensor_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    pHeightSensor = new Sensor<HeightT>(
        99, "Height", SensorDeviceClass::distance, Unit::Type::cm);
    pPbs = new PresenceBinarySensor(89, "PresenceBinarySensor", *pHeightSensor);
  }

  void TearDown() override {
    delete pPbs;
    delete pHeightSensor;
    releaseArduinoMock();
  }

  ArduinoMock* pArduinoMock;
  Sensor<HeightT>* pHeightSensor;
  PresenceBinarySensor* pPbs;
};

TEST_F(PresenceBinarySensor_test, callService_shall_do_nothing) {
  pPbs->callService(0);
}

TEST_F(PresenceBinarySensor_test, getConfigItemValues) {
  ConfigItemValueT items[4];

  EXPECT_EQ(pPbs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);

  EXPECT_EQ(items[0].configId, 0);
  EXPECT_EQ(items[0].value, 180);

  EXPECT_EQ(items[1].configId, 1);
  EXPECT_EQ(items[1].value, 200);

  EXPECT_EQ(items[2].configId, 2);
  EXPECT_EQ(items[2].value, 10000);

  EXPECT_EQ(items[3].configId, 3);
  EXPECT_EQ(items[3].value, 60);
}

TEST_F(PresenceBinarySensor_test, getDiscoveryItem) {
  DiscoveryItemT item;

  pPbs->getDiscoveryItem(&item);

  EXPECT_EQ(item.entity.entityId, 89);
  EXPECT_EQ(item.entity.componentType,
            static_cast<uint8_t>(BaseComponent::Type::binarySensor));
  EXPECT_EQ(item.entity.deviceClass,
            static_cast<uint8_t>(BinarySensorDeviceClass::presence));
  EXPECT_EQ(item.entity.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_FALSE(item.entity.isSigned);
  EXPECT_EQ(item.entity.size, 1);
  EXPECT_EQ(item.entity.precision, 0);

  EXPECT_EQ(item.numberOfConfigItems, 4);

  EXPECT_EQ(item.configItems[0].configId, 0);
  EXPECT_EQ(item.configItems[0].unit, static_cast<uint8_t>(Unit::Type::cm));
  EXPECT_EQ(item.configItems[0].size, sizeof(HeightT) / 2);
  EXPECT_TRUE(item.configItems[0].isSigned);
  EXPECT_EQ(item.configItems[0].precision, 0);

  EXPECT_EQ(item.configItems[1].configId, 1);
  EXPECT_EQ(item.configItems[1].unit, static_cast<uint8_t>(Unit::Type::cm));
  EXPECT_TRUE(item.configItems[1].isSigned);
  EXPECT_EQ(item.configItems[1].size, sizeof(HeightT) / 2);
  EXPECT_EQ(item.configItems[1].precision, 0);

  EXPECT_EQ(item.configItems[2].configId, 2);
  EXPECT_EQ(item.configItems[2].unit, static_cast<uint8_t>(Unit::Type::ms));
  EXPECT_FALSE(item.configItems[2].isSigned);
  EXPECT_EQ(item.configItems[2].size, sizeof(uint16_t) / 2);
  EXPECT_EQ(item.configItems[2].precision, 0);

  EXPECT_EQ(item.configItems[3].configId, 3);
  EXPECT_EQ(item.configItems[3].unit, static_cast<uint8_t>(Unit::Type::s));
  EXPECT_FALSE(item.configItems[3].isSigned);
  EXPECT_EQ(item.configItems[3].size, sizeof(uint16_t) / 2);
  EXPECT_EQ(item.configItems[3].precision, 0);
}

TEST_F(PresenceBinarySensor_test, getEntityId) {
  EXPECT_EQ(pPbs->getEntityId(), 89);
}

TEST_F(PresenceBinarySensor_test, getValueItem) {
  ValueItemT item;

  pPbs->getValueItem(&item);

  EXPECT_EQ(item.entityId, 89);
  EXPECT_EQ(item.value, 0);
}

TEST_F(PresenceBinarySensorPrint_test, print) {
  const char* expectStr = "PresenceBinarySensor: away";
  Sensor<HeightT>* pHeightSensor = new Sensor<HeightT>(
      99, "Height", SensorDeviceClass::distance, Unit::Type::cm);
  PresenceBinarySensor pbs =
      PresenceBinarySensor(89, "PresenceBinarySensor", *pHeightSensor);

  EXPECT_EQ(pbs.print(*pSerial), strlen(expectStr));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
}

TEST_F(PresenceBinarySensorPrint_test, print_service_shall_do_nothing) {
  Sensor<HeightT>* pHeightSensor = new Sensor<HeightT>(
      99, "Height", SensorDeviceClass::distance, Unit::Type::cm);
  PresenceBinarySensor pbs =
      PresenceBinarySensor(89, "PresenceBinarySensor", *pHeightSensor);
  EXPECT_EQ(pbs.print(*pSerial, 0), 0);
}

TEST_F(PresenceBinarySensor_test, setConfigs_all_in_order) {
  ConfigItemValueT inItems[4] = {{0, 1000}, {1, 1001}, {2, 1002}, {3, 1003}};
  EXPECT_TRUE(pPbs->setConfigItemValues(inItems, 4));

  ConfigItemValueT items[4];
  EXPECT_EQ(pPbs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);
  EXPECT_EQ(items[0].value, 1000);
  EXPECT_EQ(items[1].value, 1001);
  EXPECT_EQ(items[2].value, 1002);
  EXPECT_EQ(items[3].value, 1003);
}

TEST_F(PresenceBinarySensor_test, setConfigs_all_out_of_order) {
  ConfigItemValueT inItems[4] = {{3, 2003}, {2, 2002}, {1, 2001}, {0, 2000}};
  EXPECT_TRUE(pPbs->setConfigItemValues(inItems, 4));

  ConfigItemValueT items[4];
  EXPECT_EQ(pPbs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);
  EXPECT_EQ(items[0].value, 2000);
  EXPECT_EQ(items[1].value, 2001);
  EXPECT_EQ(items[2].value, 2002);
  EXPECT_EQ(items[3].value, 2003);
}

TEST_F(PresenceBinarySensor_test, setConfigs_one) {
  ConfigItemValueT inItems[1] = {{2, 3002}};
  EXPECT_TRUE(pPbs->setConfigItemValues(inItems, 1));

  ConfigItemValueT items[4];
  EXPECT_EQ(pPbs->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            4);
  EXPECT_EQ(items[0].value, 180);
  EXPECT_EQ(items[1].value, 200);
  EXPECT_EQ(items[2].value, 3002);
  EXPECT_EQ(items[3].value, 60);
}

TEST_F(PresenceBinarySensor_test, setConfigs_too_many) {
  ConfigItemValueT inItems[5] = {
      {0, 1000}, {1, 1001}, {2, 1002}, {3, 1003}, {4, 1004}};
  EXPECT_FALSE(pPbs->setConfigItemValues(inItems, 5));
}

TEST_F(PresenceBinarySensor_test, setConfigs_out_of_range) {
  ConfigItemValueT inItems[1] = {{4, 3004}};
  EXPECT_FALSE(pPbs->setConfigItemValues(inItems, 1));
}

TEST_F(PresenceBinarySensor_test, setReported) {
  EXPECT_CALL(*pArduinoMock, millis()).Times(1);
  pPbs->setReported();
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowLowLimit_belowMinStableTime_aboveLastReportedTimeLimit_zeroMinReportTime_shall_return_false) {
  pHeightSensor->setValue(179);
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(9999));
  ConfigItemValueT inItems[1] = {{3, 0}};
  EXPECT_TRUE(pPbs->setConfigItemValues(inItems, 1));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowLowLimit_belowMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  pHeightSensor->setValue(179);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_aboveLowLimit_belowMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  pHeightSensor->setValue(180);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowHighLimit_belowMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  pHeightSensor->setValue(200);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_aboveHighLimit_belowMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  pHeightSensor->setValue(201);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowLowLimit_aboveMinStableTime_belowLastReportedTimeLimit_shall_return_true) {
  pHeightSensor->setValue(179);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(10000))
      .WillOnce(Return(59999));
  EXPECT_TRUE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_aboveLowLimit_aboveMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  pHeightSensor->setValue(180);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(10000))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowHighLimit_aboveMinStableTime_belowLastReportedTimeLimit_shall_return_false) {
  pHeightSensor->setValue(200);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(10000))
      .WillOnce(Return(59999));
  EXPECT_FALSE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_aboveHighLimit_aboveMinStableTime_belowLastReportedTimeLimit_shall_return_true) {
  pHeightSensor->setValue(201);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(10000))
      .WillOnce(Return(59999));
  EXPECT_TRUE(pPbs->update());
}

TEST_F(
    PresenceBinarySensor_test,
    update_belowLowLimit_belowMinStableTime_aboveLastReportedTimeLimit_shall_return_true) {
  pHeightSensor->setValue(179);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(9999))
      .WillOnce(Return(60000));
  EXPECT_TRUE(pPbs->update());
}
