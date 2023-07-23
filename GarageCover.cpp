#include "GarageCover.h"

#include <Arduino.h>
#include <assert.h>

#include "Cover.h"
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
  CoverState state = mCover.getState();

  switch (mCover.serviceDecode(service)) {
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

    default:
      break;
  }
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

  return mCover.getState();  // Error condition, both sensors can't be LOW.
}

void GarageCover::getDiscoveryItem(DiscoveryItemT* item) const {
  mCover.getDiscoveryEntityItem(&item->entity);
  item->numberOfConfigItems = 0;
}

bool GarageCover::isClosed(bool closedSensor, bool openSensor) {
  return closedSensor == LOW && openSensor == HIGH;
}

bool GarageCover::isClosing(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == HIGH &&
         (mCover.getState() == CoverState::closing ||
          mCover.getState() == CoverState::open);
}

bool GarageCover::isOpen(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == LOW;
}

bool GarageCover::isOpening(bool closedSensor, bool openSensor) {
  return closedSensor == HIGH && openSensor == HIGH &&
         (mCover.getState() == CoverState::opening ||
          mCover.getState() == CoverState::closed);
}

bool GarageCover::update() {
  CoverState newState = determineState();
  bool hasChanged = newState != mCover.getState();

  mCover.setState(newState);

  bool isReportDue = hasChanged;
  mCover.setIsReportDue(isReportDue);

  return isReportDue;
}
