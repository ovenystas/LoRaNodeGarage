/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "GarageCover.h"
#include "Util.h"
#include "components/Cover.h"

bool GarageCover::isClosed(bool closedSensor, bool openSensor) {
  return closedSensor == LOW && openSensor == HIGH;
}

bool GarageCover::isOpen(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == LOW;
}

bool GarageCover::isClosing(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == HIGH
      && (mState == Cover::State::closing || mState == Cover::State::open);
}

bool GarageCover::isOpening(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == HIGH
      && (mState == Cover::State::opening || mState == Cover::State::closed);
}

Cover::State GarageCover::determineState(bool closedSensor, bool openSensor) {
  if (isClosed(closedSensor, openSensor)) {
    return Cover::State::closed;
  }

  if (isOpen(closedSensor, openSensor)) {
    return Cover::State::open;
  }

  if (isClosing(closedSensor, openSensor)) {
    return Cover::State::closing;
  }

  if (isOpening(closedSensor, openSensor)) {
    return Cover::State::opening;
  }

  return mState; // Error condition, both sensors can't be LOW.
}

bool GarageCover::update() {
  bool closedSensor = digitalRead(mPinClosed);
  bool openSensor = digitalRead(mPinOpen);

  State newState = determineState(closedSensor, openSensor);
  bool hasChanged = newState != mState;

  mState = newState;

  return hasChanged;
}

void GarageCover::callService(const Cover::Service service) {
  bool closedSensor = digitalRead(mPinClosed);
  bool openSensor = digitalRead(mPinOpen);
  State state = determineState(closedSensor, openSensor);

  switch (service) {
    case Cover::Service::open:
      if (state == Cover::State::closed) {
        // relay 1 click
      } else if (state == Cover::State::closing) {
        // relay 2 click
      }
      break;

    case Cover::Service::close:
      if (state == Cover::State::open) {
        // relay 1 click
      } else if (state == Cover::State::opening) {
        // relay 2 click
      }
      break;

    case Cover::Service::stop:
      if (state == Cover::State::opening || state == Cover::State::closing) {
        // relay 1 click
      }
      break;

    case Cover::Service::toggle:
      if (state == Cover::State::open || state == Cover::State::closed) {
        // relay 1 click
      } else if (state == Cover::State::opening || state == Cover::State::closing) {
        // relay 2 click
      }
      break;
  }
}
