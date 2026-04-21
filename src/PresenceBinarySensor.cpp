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

bool PresenceBinarySensor::getDiscoveryEntity(DiscoveryEntityT& item) const {
  mBinarySensor.getDiscoveryEntity(item);
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
