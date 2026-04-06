#include "PresenceBinarySensor.h"

#include <Arduino.h>
#include <assert.h>

#include "Ee.h"
#include "Util.h"

bool PresenceBinarySensor::update() {
  uint32_t timestamp = millis();

  HeightT height = mHeightSensor.getValue();

  bool newState =
      (height >= mLowLimit.getValue()) && (height <= mHighLimit.getValue());

  if (newState != mBinarySensor.getState()) {
    mLastChangedTime = timestamp;
    mStableState = false;
  }

  mBinarySensor.setState(newState);

  bool enteredNewStableState = !mStableState && timestamp - mLastChangedTime >=
                                                    mMinStableTime.getValue();

  if (enteredNewStableState) {
    mStableState = true;
  }

  bool timeToReport = (mBinarySensor.timeSinceLastReport() / 1000) >=
                      HeightSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT;

  bool isReportDue = enteredNewStableState || timeToReport;
  mBinarySensor.setIsReportDue(isReportDue);

  return isReportDue;
}

bool PresenceBinarySensor::getConfigValue(ValueItemT& item,
                                          uint8_t index) const {
  assert(index < sNumConfigItems);

  switch (index) {
    case 0:
      mLowLimit.getValueItem(item);
      break;
    case 1:
      mHighLimit.getValueItem(item);
      break;
    case 2:
      mMinStableTime.getValueItem(item);
      break;
    default:
      return false;
  }

  return true;
}

bool PresenceBinarySensor::getDiscoveryEntity(DiscoveryEntityT& item,
                                              uint8_t index) const {
  switch (index) {
    case 0:
      mBinarySensor.getDiscoveryEntity(item);
      break;
    case 1:
      mLowLimit.getDiscoveryEntity(item);
      break;
    case 2:
      mHighLimit.getDiscoveryEntity(item);
      break;
    case 3:
      mMinStableTime.getDiscoveryEntity(item);
      break;
    default:
      return false;
  }

  return true;
}

bool PresenceBinarySensor::setValueItem(const ValueItemT& item) {
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
    default:
      return false;
  }

  return true;
}

void PresenceBinarySensor::loadConfigValues() {
  mLowLimit.loadFromEeprom(
      PresenceBinarySensorConstants::CONFIG_LOW_LIMIT_DEFAULT);
  mHighLimit.loadFromEeprom(
      PresenceBinarySensorConstants::CONFIG_HIGH_LIMIT_DEFAULT);
  mMinStableTime.loadFromEeprom(
      PresenceBinarySensorConstants::CONFIG_MIN_STABLE_TIME_DEFAULT);
}
