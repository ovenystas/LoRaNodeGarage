/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "GarageCover.h"
#include "Util.h"

bool GarageCover::isClosed(bool closedSensor, bool openSensor) {
  return closedSensor == LOW && openSensor == HIGH;
}

bool GarageCover::isOpen(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == LOW;
}

bool GarageCover::isClosing(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == HIGH
      && (mState == Cover::STATE_CLOSING || mState == Cover::STATE_OPEN);
}

bool GarageCover::isOpening(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == HIGH
      && (mState == Cover::STATE_OPENING || mState == Cover::STATE_CLOSED);
}

Cover::StateE GarageCover::determineState(bool closedSensor, bool openSensor) {
  if (isClosed(closedSensor, openSensor)) {
    return Cover::STATE_CLOSED;
  }

  if (isOpen(closedSensor, openSensor)) {
    return Cover::STATE_OPEN;
  }

  if (isClosing(closedSensor, openSensor)) {
    return Cover::STATE_CLOSING;
  }

  if (isOpening(closedSensor, openSensor)) {
    return Cover::STATE_OPENING;
  }

  return mState; // Error condition, both sensors can't be LOW.
}

bool GarageCover::update() {
  bool closedSensor = digitalRead(mPinClosed);
  bool openSensor = digitalRead(mPinOpen);

  StateE newState = determineState(closedSensor, openSensor);
  bool hasChanged = newState != mState;

  mState = newState;

  return hasChanged;
}
