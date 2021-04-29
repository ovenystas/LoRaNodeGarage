/*
 *  Created on: 7 mars 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "PresenceBinarySensor.h"
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

  bool enteredStableState = !mStableState
      && millis() >= mLastChangedTime + mConfig.minStableTime.getValue();

  if (enteredStableState) {
    mStableState = true;
  }

  return enteredStableState;
}

uint8_t PresenceBinarySensor::getDiscoveryMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  p += getDiscoveryMsg(p);
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
