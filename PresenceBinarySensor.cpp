/*
 *  Created on: 7 mars 2021
 *      Author: oveny
 */

#include "PresenceBinarySensor.h"

#include <Arduino.h>

#include "Util.h"

bool PresenceBinarySensor::update() {
  HeightT height = mHeightSensor.getValue();

  bool newState = (height >= mConfig.lowLimit.getValue()) &&
                  (height <= mConfig.highLimit.getValue());

  if (newState != getState()) {
    mLastChangedTime = millis();
    mStableState = false;
  }

  setState(newState);

  bool enteredStableState =
      !mStableState &&
      millis() >= mLastChangedTime + mConfig.minStableTime.getValue();

  if (enteredStableState) {
    mStableState = true;
  }

  return enteredStableState;
}

uint8_t PresenceBinarySensor::getDiscoveryMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  p += BinarySensor::getDiscoveryMsg(p);
  *p++ = mConfig.numberOfConfigItems;

  p += mConfig.lowLimit.writeDiscoveryItem(p);
  p += mConfig.highLimit.writeDiscoveryItem(p);
  p += mConfig.minStableTime.writeDiscoveryItem(p);

  return p - buffer;
}

uint8_t PresenceBinarySensor::getConfigItemValuesMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  *p++ = getEntityId();
  *p++ = mConfig.numberOfConfigItems;

  p += mConfig.lowLimit.writeConfigItemValue(p);
  p += mConfig.highLimit.writeConfigItemValue(p);
  p += mConfig.minStableTime.writeConfigItemValue(p);

  return p - buffer;
}

void PresenceBinarySensor::setConfigs(uint8_t numberOfConfigs,
                                      const uint8_t* buffer) {
  if (numberOfConfigs != mConfig.numberOfConfigItems) {
    return;
  }
  const uint8_t* p = buffer;
  p += mConfig.lowLimit.setConfigValue(p[0], &p[1]);
  p += mConfig.highLimit.setConfigValue(p[0], &p[1]);
  p += mConfig.minStableTime.setConfigValue(p[0], &p[1]);
}
