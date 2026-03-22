#include "PresenceBinarySensor.h"

#include <Arduino.h>
#include <assert.h>

#include "Ee.h"
#include "Util.h"

bool PresenceBinarySensor::update() {
  uint32_t timestamp = millis();

  HeightT height = mHeightSensor.getValue();

  bool newState = (height >= mLowLimit.getValue()) &&
                  (height <= mHighLimit.getValue());

  if (newState != mBinarySensor.getState()) {
    mLastChangedTime = timestamp;
    mStableState = false;
  }

  mBinarySensor.setState(newState);

  bool enteredNewStableState =
      !mStableState &&
      timestamp - mLastChangedTime >= mMinStableTime.getValue();

  if (enteredNewStableState) {
    mStableState = true;
  }

  bool timeToReport = mReportInterval.getValue() > 0
                          ? (mBinarySensor.timeSinceLastReport() / 1000) >=
                                mReportInterval.getValue()
                          : false;

  bool isReportDue = enteredNewStableState || timeToReport;
  mBinarySensor.setIsReportDue(isReportDue);

  return isReportDue;
}

uint8_t PresenceBinarySensor::getConfigValueItems(ValueItemT* items, uint8_t length) const {
  assert(sNumConfigItems <= length);

  mLowLimit.getValueItem(&items[0]);
  mHighLimit.getValueItem(&items[1]);
  mMinStableTime.getValueItem(&items[2]);
  mReportInterval.getValueItem(&items[3]);

  return sNumConfigItems;
}

uint8_t PresenceBinarySensor::getDiscoveryItems(DiscoveryEntityItemT* items, uint8_t length) const {
  assert(sNumItems <= length);

  mBinarySensor.getDiscoveryEntityItem(&items[0]);
  mLowLimit.getDiscoveryEntityItem(&items[1]);
  mHighLimit.getDiscoveryEntityItem(&items[2]);
  mMinStableTime.getDiscoveryEntityItem(&items[3]);
  mReportInterval.getDiscoveryEntityItem(&items[4]);

  return sNumItems;
}

bool PresenceBinarySensor::setValueItem(const ValueItemT &item) {
  switch (item.entityId - mBinarySensor.getEntityId() - 1) {
    case 0:
      mLowLimit.setValueItem(item);
      break;
    case 1:
      mHighLimit.setValueItem(item);
      break;
    case 2:
      mMinStableTime.setValueItem(item);
      break;
    case 3:
      mReportInterval.setValueItem(item);
      break;
    default:
      return false;
  }

  return true;
}

void PresenceBinarySensor::loadConfigValues() {
  Ee::loadValue(EE_ADDRESS_CONFIG_PRESENCE_BINARY_SENSOR_0, mLowLimit,
    PresenceBinarySensorConstants::CONFIG_LOW_LIMIT_DEFAULT);

  Ee::loadValue(EE_ADDRESS_CONFIG_PRESENCE_BINARY_SENSOR_1, mHighLimit,
    PresenceBinarySensorConstants::CONFIG_HIGH_LIMIT_DEFAULT);

  Ee::loadValue(EE_ADDRESS_CONFIG_PRESENCE_BINARY_SENSOR_2, mMinStableTime,
    PresenceBinarySensorConstants::CONFIG_MIN_STABLE_TIME_DEFAULT);

  Ee::loadValue(EE_ADDRESS_CONFIG_PRESENCE_BINARY_SENSOR_3, mReportInterval,
    PresenceBinarySensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT);
}
