#include "HeightSensor.h"

#include <Arduino.h>
#include <assert.h>

#include "Ee.h"
#include "Sensor.h"
#include "Util.h"

bool HeightSensor::update() {
  HeightT newValue = mZeroValue.getValue() - mDistanceSensor.getValue();

  mSensor.setValue(newValue);

  bool largeChange = mReportHysteresis.getValue() > 0
                         ? mSensor.absDiffLastReportedValue() >=
                               mReportHysteresis.getValue()
                         : false;

  bool timeToReport = mReportInterval.getValue() > 0
                          ? (mSensor.timeSinceLastReport() / 1000) >=
                                mReportInterval.getValue()
                          : false;

  bool isReportDue = largeChange || timeToReport;
  mSensor.setIsReportDue(isReportDue);

  return isReportDue;
}

uint8_t HeightSensor::getDiscoveryItems(DiscoveryEntityItemT* items, uint8_t length) const {
  assert(sNumItems <= length);

  mSensor.getDiscoveryEntityItem(&items[0]);
  mReportHysteresis.getDiscoveryEntityItem(&items[1]);
  mReportInterval.getDiscoveryEntityItem(&items[2]);
  mStableTime.getDiscoveryEntityItem(&items[3]);
  mZeroValue.getDiscoveryEntityItem(&items[4]);

  return sNumItems;
}

uint8_t HeightSensor::getConfigValueItems(ValueItemT* items, uint8_t length) const {
  assert(sNumConfigItems <= length);

  mReportHysteresis.getValueItem(&items[0]);
  mReportInterval.getValueItem(&items[1]);
  mStableTime.getValueItem(&items[2]);
  mZeroValue.getValueItem(&items[3]);

  return sNumConfigItems;
}

bool HeightSensor::setValueItem(const ValueItemT &item) {
    switch (item.entityId - mSensor.getEntityId() - 1) {
      case 0:
        mReportHysteresis.setValueItem(item);
        break;
      case 1:
        mReportInterval.setValueItem(item);
        break;
      case 2:
        mStableTime.setValueItem(item);
        break;
      case 3:
        mZeroValue.setValueItem(item);
        break;
      default:
        return false;
    }

  return true;
}

void HeightSensor::loadConfigValues() {
    Ee::loadValue(EE_ADDRESS_CONFIG_HEIGHT_SENSOR_0, mReportHysteresis,
      HeightSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT);

    Ee::loadValue(EE_ADDRESS_CONFIG_HEIGHT_SENSOR_1, mReportInterval,
      HeightSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT);

    Ee::loadValue(EE_ADDRESS_CONFIG_HEIGHT_SENSOR_2, mStableTime,
      HeightSensorConstants::CONFIG_STABLE_TIME_DEFAULT);

    Ee::loadValue(EE_ADDRESS_CONFIG_HEIGHT_SENSOR_3, mZeroValue,
      HeightSensorConstants::CONFIG_ZERO_VALUE_DEFAULT);
}
