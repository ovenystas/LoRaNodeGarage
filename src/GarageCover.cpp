#include "GarageCover.h"

#include <Arduino.h>
#include <assert.h>

#include "Cover.h"
#include "Ee.h"
#include "Util.h"

void GarageCover::activateRelay(uint8_t times) {
  assert(times <= 2);

  for (uint8_t i = 0; i < times; i++) {
    digitalWrite(mPinRelay, HIGH);
    delay(500);
    digitalWrite(mPinRelay, LOW);
    if (i < times - 1) {
      delay(500);
    }
  }
}

void GarageCover::callService(uint8_t service) {
  const CoverState state = mCover.getState();

  switch (mCover.serviceDecode(service)) {
    case CoverService::OPEN:
      if (state == CoverState::CLOSED) {
        activateRelay(1);
      } else if (state == CoverState::CLOSING) {
        activateRelay(2);
      }
      break;

    case CoverService::CLOSE:
      if (state == CoverState::OPEN) {
        activateRelay(1);
      } else if (state == CoverState::OPENING) {
        activateRelay(2);
      }
      break;

    case CoverService::STOP:
      if (state == CoverState::OPENING || state == CoverState::CLOSING) {
        activateRelay(1);
      }
      break;

    case CoverService::TOGGLE:
      if (state == CoverState::OPEN || state == CoverState::CLOSED) {
        activateRelay(1);
      } else if (state == CoverState::OPENING || state == CoverState::CLOSING) {
        activateRelay(2);
      }
      break;

    default:
      break;
  }
}

CoverState GarageCover::determineState() {
  const bool closedSensor = digitalRead(mPinClosed);
  const bool openSensor = digitalRead(mPinOpen);

  if (isClosed(closedSensor, openSensor)) {
    return CoverState::CLOSED;
  }

  if (isOpen(closedSensor, openSensor)) {
    return CoverState::OPEN;
  }

  if (isClosing(closedSensor, openSensor)) {
    return CoverState::CLOSING;
  }

  if (isOpening(closedSensor, openSensor)) {
    return CoverState::OPENING;
  }

  return mCover.getState();  // Error condition, both sensors can't be LOW.
}

bool GarageCover::getDiscoveryEntity(DiscoveryEntityT& item) const {
  mCover.getDiscoveryEntity(item);
  return true;
}

bool GarageCover::getConfigValue(ValueItemT& item, uint8_t index) const {
  if (index >= sNumConfigItems) {
    return false;
  }

  // GarageCover has no config items

  return true;
}

bool GarageCover::setValueItem(const ValueItemT& item) { return false; }

bool GarageCover::isClosed(bool closedSensor, bool openSensor) {
  return closedSensor == LOW && openSensor == HIGH;
}

bool GarageCover::isClosing(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == HIGH &&
         (mCover.getState() == CoverState::CLOSING ||
          mCover.getState() == CoverState::OPEN);
}

bool GarageCover::isOpen(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == LOW;
}

bool GarageCover::isOpening(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == HIGH &&
         (mCover.getState() == CoverState::OPENING ||
          mCover.getState() == CoverState::CLOSED);
}

bool GarageCover::update() {
  const CoverState newState = determineState();
  const bool hasChanged = (newState != mCover.getState());

  mCover.setState(newState);

  bool isReportDue = hasChanged;
  mCover.setIsReportDue(isReportDue);

  return isReportDue;
}

void GarageCover::loadConfigValues() {}
