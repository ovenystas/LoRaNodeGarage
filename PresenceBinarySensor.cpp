/*
 *  Created on: 7 mars 2021
 *      Author: oveny
 */

#include "PresenceBinarySensor.h"
#include <Arduino.h>
#include "Util.h"

bool PresenceBinarySensor::update() {
  auto height = mHeightSensor.getValue();
  auto newState = (height >= mConfig.lowLimit) && (height <= mConfig.highLimit);

  bool hasChanged = newState != mState;
  if (hasChanged) {
    mLastChangedTime = millis();
    mStableState = false;
  }

  mState = newState;

  bool enteredStableState =
      !mStableState && millis() >= mLastChangedTime + mConfig.minTime;
  if (enteredStableState) {
    mStableState = true;
  }

  return enteredStableState;
}
