#include "BinarySensor.h"

#include <WString.h>

#include "Component.h"
#include "Unit.h"

const __FlashStringHelper* BinarySensor::getStateName() {
  switch (getDeviceClass()) {
    case BinarySensorDeviceClass::battery:
      return mState ? F("low") : F("normal");

    case BinarySensorDeviceClass::cold:
      return mState ? F("cold") : F("normal");

    case BinarySensorDeviceClass::heat:
      return mState ? F("hot") : F("normal");

    case BinarySensorDeviceClass::connectivity:
      return mState ? F("connected") : F("disconnected");

    case BinarySensorDeviceClass::door:
    case BinarySensorDeviceClass::garageDoor:
    case BinarySensorDeviceClass::opening:
    case BinarySensorDeviceClass::window:
      return mState ? F("open") : F("closed");

    case BinarySensorDeviceClass::lock:
      return mState ? F("unlocked") : F("locked");

    case BinarySensorDeviceClass::moisture:
      return mState ? F("wet") : F("dry");

    case BinarySensorDeviceClass::gas:
    case BinarySensorDeviceClass::motion:
    case BinarySensorDeviceClass::occupancy:
    case BinarySensorDeviceClass::smoke:
    case BinarySensorDeviceClass::sound:
    case BinarySensorDeviceClass::vibration:
      return mState ? F("detected") : F("clear");

    case BinarySensorDeviceClass::presence:
      return mState ? F("home") : F("away");

    case BinarySensorDeviceClass::problem:
      return mState ? F("problem") : F("OK");

    case BinarySensorDeviceClass::safety:
      return mState ? F("unsafe") : F("safe");

    default:
      return mState ? F("on") : F("off");
  }
}

uint8_t BinarySensor::getDiscoveryMsg(uint8_t* buffer) {
  buffer[0] = getEntityId();
  buffer[1] = static_cast<uint8_t>(getComponentType());
  buffer[2] = static_cast<uint8_t>(getDeviceClass());
  buffer[3] = static_cast<uint8_t>(mUnit.getType());
  buffer[4] = (1 << 4) | 0;
  return 5;
}

uint8_t BinarySensor::getValueMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  *p++ = getEntityId();
  *p++ = getState();

  return p - buffer;
}

void BinarySensor::print(Stream& stream) {
  stream.print(getName());
  stream.print(": ");
  stream.print(getStateName());
}
