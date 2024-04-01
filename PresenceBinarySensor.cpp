/*
 *  Created on: 7 mars 2021
 *      Author: oveny
 */

#include "PresenceBinarySensor.h"

#include <Arduino.h>
#include <assert.h>

#include "Util.h"

bool PresenceBinarySensor::update() {
  uint32_t timestamp = millis();

  HeightT height = mHeightSensor.getValue();

  bool newState = (height >= mConfig.lowLimit.getValue()) &&
                  (height <= mConfig.highLimit.getValue());

  if (newState != mBinarySensor.getState()) {
    mLastChangedTime = timestamp;
    mStableState = false;
  }

  mBinarySensor.setState(newState);

  bool enteredNewStableState =
      !mStableState &&
      timestamp - mLastChangedTime >= mConfig.minStableTime.getValue();

  if (enteredNewStableState) {
    mStableState = true;
  }

  bool timeToReport = mConfig.reportInterval.getValue() > 0
                          ? mBinarySensor.timeSinceLastReport() >=
                                (mConfig.reportInterval.getValue() * 1000)
                          : false;

  bool isReportDue = enteredNewStableState || timeToReport;
  mBinarySensor.setIsReportDue(isReportDue);

  return isReportDue;
}

uint8_t PresenceBinarySensor::getConfigItemValues(ConfigItemValueT* items,
                                                  uint8_t length) const {
  assert(mConfig.numberOfConfigItems <= length);

  mConfig.lowLimit.getConfigItemValue(&items[0]);
  mConfig.highLimit.getConfigItemValue(&items[1]);
  mConfig.minStableTime.getConfigItemValue(&items[2]);
  mConfig.reportInterval.getConfigItemValue(&items[3]);

  return mConfig.numberOfConfigItems;
}

void PresenceBinarySensor::getDiscoveryItem(DiscoveryItemT* item) const {
  assert(mConfig.numberOfConfigItems <=
         sizeof(item->configItems) / sizeof(item->configItems[0]));

  mBinarySensor.getDiscoveryEntityItem(&item->entity);
  item->numberOfConfigItems = mConfig.numberOfConfigItems;
  mConfig.lowLimit.getDiscoveryConfigItem(&item->configItems[0]);
  mConfig.highLimit.getDiscoveryConfigItem(&item->configItems[1]);
  mConfig.minStableTime.getDiscoveryConfigItem(&item->configItems[2]);
  mConfig.reportInterval.getDiscoveryConfigItem(&item->configItems[3]);
}

bool PresenceBinarySensor::setConfigItemValues(const ConfigItemValueT* items,
                                               uint8_t length) {
  if (length > mConfig.numberOfConfigItems) {
    return false;
  }

  for (uint8_t i = 0; i < length; i++) {
    switch (items[i].configId) {
      case 0:
        (void)mConfig.lowLimit.setConfigItemValue(&items[i]);
        break;
      case 1:
        (void)mConfig.highLimit.setConfigItemValue(&items[i]);
        break;
      case 2:
        (void)mConfig.minStableTime.setConfigItemValue(&items[i]);
        break;
      case 3:
        (void)mConfig.reportInterval.setConfigItemValue(&items[i]);
        break;
      default:
        return false;
    }
  }

  return true;
}

void PresenceBinarySensor::loadConfigValues() {
  mConfig.lowLimit.load();
  mConfig.highLimit.load();
  mConfig.minStableTime.load();
  mConfig.reportInterval.load();
}
