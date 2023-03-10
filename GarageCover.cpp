#include "GarageCover.h"

#include <Arduino.h>

#include "Cover.h"
#include "Util.h"

bool GarageCover::isClosed(bool closedSensor, bool openSensor) {
  return closedSensor == LOW && openSensor == HIGH;
}

bool GarageCover::isOpen(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == LOW;
}

bool GarageCover::isClosing(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == HIGH &&
         (getState() == CoverState::closing || getState() == CoverState::open);
}

bool GarageCover::isOpening(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == HIGH &&
         (getState() == CoverState::opening ||
          getState() == CoverState::closed);
}

CoverState GarageCover::determineState() {
  bool closedSensor = digitalRead(mPinClosed);
  bool openSensor = digitalRead(mPinOpen);

  if (isClosed(closedSensor, openSensor)) {
    return CoverState::closed;
  }

  if (isOpen(closedSensor, openSensor)) {
    return CoverState::open;
  }

  if (isClosing(closedSensor, openSensor)) {
    return CoverState::closing;
  }

  if (isOpening(closedSensor, openSensor)) {
    return CoverState::opening;
  }

  return getState();  // Error condition, both sensors can't be LOW.
}

bool GarageCover::update() {
  CoverState newState = determineState();
  bool hasChanged = newState != getState();

  setState(newState);

  return hasChanged;
}

uint8_t GarageCover::getDiscoveryMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  p += Cover::getDiscoveryMsg(p);
  *p++ = 0;  // No config items

  return p - buffer;
}

void GarageCover::callService(const CoverService service) {
  CoverState state = getState();

  switch (service) {
    case CoverService::open:
      if (state == CoverState::closed) {
        activateRelay(1);
      } else if (state == CoverState::closing) {
        activateRelay(2);
      }
      break;

    case CoverService::close:
      if (state == CoverState::open) {
        activateRelay(1);
      } else if (state == CoverState::opening) {
        activateRelay(2);
      }
      break;

    case CoverService::stop:
      if (state == CoverState::opening || state == CoverState::closing) {
        activateRelay(1);
      }
      break;

    case CoverService::toggle:
      if (state == CoverState::open || state == CoverState::closed) {
        activateRelay(1);
      } else if (state == CoverState::opening || state == CoverState::closing) {
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
