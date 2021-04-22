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
  uint8_t length = BinarySensor::getDiscoveryMsg(buffer);
  uint8_t& numberOfConfigs = buffer[length++];

  length += mConfig.lowLimit.writeDiscoveryItem(&buffer[length]);
  length += mConfig.highLimit.writeDiscoveryItem(&buffer[length]);
  length += mConfig.minStableTime.writeDiscoveryItem(&buffer[length]);

  numberOfConfigs = 3;

  return length;
}
