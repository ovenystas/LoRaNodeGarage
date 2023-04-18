/*
 *  Created on: 7 mars 2021
 *      Author: oveny
 */

#include "PresenceBinarySensor.h"

#include <Arduino.h>

#include "Util.h"

bool PresenceBinarySensor::update() {
  uint32_t timestamp = millis();

  HeightT height = mHeightSensor.getValue();

  bool newState = (height >= mConfig.lowLimit.getValue()) &&
                  (height <= mConfig.highLimit.getValue());

  if (newState != getState()) {
    mLastChangedTime = timestamp;
    mStableState = false;
  }

  setState(newState);

  bool enteredNewStableState =
      !mStableState &&
      timestamp >= mLastChangedTime + mConfig.minStableTime.getValue();

  if (enteredNewStableState) {
    mStableState = true;
  }

  bool reportIsDue =
      mConfig.reportInterval.getValue() > 0
          ? timeSinceLastReport() >= mConfig.reportInterval.getValue()
          : false;

  return (enteredNewStableState || reportIsDue);
}

uint8_t PresenceBinarySensor::getDiscoveryMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  p += BinarySensor::getDiscoveryMsg(p);
  *p++ = mConfig.numberOfConfigItems;

  p += mConfig.lowLimit.writeDiscoveryItem(p);
  p += mConfig.highLimit.writeDiscoveryItem(p);
  p += mConfig.minStableTime.writeDiscoveryItem(p);
  p += mConfig.reportInterval.writeDiscoveryItem(p);

  return p - buffer;
}

uint8_t PresenceBinarySensor::getConfigItemValuesMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  *p++ = getEntityId();
  *p++ = mConfig.numberOfConfigItems;

  p += mConfig.lowLimit.writeConfigItemValue(p);
  p += mConfig.highLimit.writeConfigItemValue(p);
  p += mConfig.minStableTime.writeConfigItemValue(p);
  p += mConfig.reportInterval.writeConfigItemValue(p);

  return p - buffer;
}

bool PresenceBinarySensor::setConfigs(uint8_t numberOfConfigs,
                                      const uint8_t* buffer) {
  if (numberOfConfigs > mConfig.numberOfConfigItems) {
    return false;
  }

  const uint8_t* p = buffer;
  while (numberOfConfigs-- > 0) {
    switch (*p) {
      case 0:
        p += mConfig.lowLimit.setConfigValue(p[0], &p[1]);
        break;
      case 1:
        p += mConfig.highLimit.setConfigValue(p[0], &p[1]);
        break;
      case 2:
        p += mConfig.minStableTime.setConfigValue(p[0], &p[1]);
        break;
      case 3:
        p += mConfig.reportInterval.setConfigValue(p[0], &p[1]);
        break;
      default:
        return false;
    }
  }

  return true;
}
