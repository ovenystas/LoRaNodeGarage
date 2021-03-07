/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "GarageCover.h"
#include "Util.h"

bool GarageCover::update() {
  auto closedSensor = digitalRead(mPinClosed);
  auto openSensor = digitalRead(mPinOpen);

  StateE newState;
  if (closedSensor == LOW && openSensor == HIGH) {
    newState = Cover::STATE_CLOSED;
  }
  else if (closedSensor == HIGH && openSensor == LOW) {
    newState = Cover::STATE_OPEN;
  }
  else if (closedSensor == HIGH && openSensor == HIGH) {
    if (mState == Cover::STATE_CLOSED) {
      newState = Cover::STATE_OPENING;
    }
    else if (mState == Cover::STATE_OPEN) {
      newState = Cover::STATE_CLOSING;
    }
    else {
      newState = mState; // Still in same state, OPENING or CLOSING.
    }
  }
  else {
    newState = mState; // Error condition, both sensors can't be LOW.
  }

  bool hasChanged = newState != mState;

  mState = newState;

  return hasChanged;
}
