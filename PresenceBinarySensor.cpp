/*
 *  Created on: 7 mars 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "PresenceBinarySensor.h"
#include "Util.h"

bool PresenceBinarySensor::update() {
  auto height = mHeightSensor.getValue();

  auto newState = (height >= mConfig.lowLimit) && (height <= mConfig.highLimit);

  if (newState != getState()) {
    mLastChangedTime = millis();
    mStableState = false;
  }

  setState(newState);

  bool enteredStableState = !mStableState
      && millis() >= mLastChangedTime + mConfig.minTime;

  if (enteredStableState) {
    mStableState = true;
  }

  return enteredStableState;
}
