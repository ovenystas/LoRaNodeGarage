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
      && (getState() == Cover::State::closing || getState() == Cover::State::open);
}

bool GarageCover::isOpening(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == HIGH
      && (getState() == Cover::State::opening || getState() == Cover::State::closed);
}

Cover::State GarageCover::determineState() {
  bool closedSensor = digitalRead(mPinClosed);
  bool openSensor = digitalRead(mPinOpen);

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

  return getState(); // Error condition, both sensors can't be LOW.
}

bool GarageCover::update() {
  State newState = determineState();
  bool hasChanged = newState != getState();

  setState(newState);

  return hasChanged;
}

uint8_t GarageCover::getDiscoveryMsg(uint8_t* buffer) {
  uint8_t length = Cover::getDiscoveryMsg(buffer);
  uint8_t& numberOfConfigs = buffer[length++];

  numberOfConfigs = 0;

  return length;
}

void GarageCover::callService(const Cover::Service service) {
  State state = determineState();

  switch (service) {
    case Cover::Service::open:
      if (state == Cover::State::closed) {
        activateRelay(1);
      } else if (state == Cover::State::closing) {
        activateRelay(2);
      }
      break;

    case Cover::Service::close:
      if (state == Cover::State::open) {
        activateRelay(1);
      } else if (state == Cover::State::opening) {
        activateRelay(2);
      }
      break;

    case Cover::Service::stop:
      if (state == Cover::State::opening || state == Cover::State::closing) {
        activateRelay(1);
      }
      break;

    case Cover::Service::toggle:
      if (state == Cover::State::open || state == Cover::State::closed) {
        activateRelay(1);
      } else if (state == Cover::State::opening || state == Cover::State::closing) {
        activateRelay(2);
      }
      break;
  }
}

void GarageCover::activateRelay(uint8_t times) {
  if (times > 2) {
    times = 2;
  }

  for (uint8_t i = 0; i < times; i++) {
    digitalWrite(mPinRelay, HIGH);
    delay(500);
    digitalWrite(mPinRelay, LOW);
    if (i < times - 1) {
      delay(500);
    }
  }
}
