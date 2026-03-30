#include "HeightSensor.h"

#include <Arduino.h>
#include <assert.h>

#include "Ee.h"
#include "Sensor.h"
#include "Util.h"

bool HeightSensor::update() {
  HeightT newValue = mZeroValue.getValue() - mDistanceSensor.getValue();

  mSensor.setValue(newValue);

  bool largeChange = mSensor.absDiffLastReportedValue() >=
                     HeightSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT;

  bool timeToReport = (mSensor.timeSinceLastReport() / 1000) >=
                      HeightSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT;

  bool isReportDue = largeChange || timeToReport;
  mSensor.setIsReportDue(isReportDue);

  return isReportDue;
}

uint8_t HeightSensor::getDiscoveryItems(DiscoveryEntityItemT* items,
                                        uint8_t length) const {
  assert(sNumItems <= length);

  mSensor.getDiscoveryEntityItem(&items[0]);
  mStableTime.getDiscoveryEntityItem(&items[3]);
  mZeroValue.getDiscoveryEntityItem(&items[4]);

  return sNumItems;
}

uint8_t HeightSensor::getConfigValueItems(ValueItemT* items,
                                          uint8_t length) const {
  assert(sNumConfigItems <= length);

  mStableTime.getValueItem(&items[2]);
  mZeroValue.getValueItem(&items[3]);

  return sNumConfigItems;
}

bool HeightSensor::setValueItem(const ValueItemT& item) {
  switch (item.entityId - mSensor.getEntityId() - 1) {
    case 0:
      mStableTime.setValueItem(item);
      break;
    case 1:
      mZeroValue.setValueItem(item);
      break;
    default:
      return false;
  }

  return true;
}

void HeightSensor::loadConfigValues() {
  Ee::loadValue(EE_ADDRESS_CONFIG_HEIGHT_SENSOR_2, mStableTime,
                HeightSensorConstants::CONFIG_STABLE_TIME_DEFAULT);

  Ee::loadValue(EE_ADDRESS_CONFIG_HEIGHT_SENSOR_3, mZeroValue,
                HeightSensorConstants::CONFIG_ZERO_VALUE_DEFAULT);
}
