/*
 *  Created on: 13 apr. 2021
 *      Author: oveny
 */

#include "BinarySensor.h"
#include "Component.h"

const __FlashStringHelper* BinarySensor::getStateName() {
  switch (getDeviceClass()) {
    case DeviceClass::battery:
      return mState ? F("low") : F("normal");

    case DeviceClass::cold:
      return mState ? F("cold") : F("normal");

    case DeviceClass::heat:
      return mState ? F("hot") : F("normal");

    case DeviceClass::connectivity:
      return mState ? F("connected") : F("disconnected");

    case DeviceClass::door:
    case DeviceClass::garageDoor:
    case DeviceClass::opening:
    case DeviceClass::window:
      return mState ? F("open") : F("closed");

    case DeviceClass::lock:
      return mState ? F("unlocked") : F("locked");

    case DeviceClass::moisture:
      return mState ? F("wet") : F("dry");

    case DeviceClass::gas:
    case DeviceClass::motion:
    case DeviceClass::occupancy:
    case DeviceClass::smoke:
    case DeviceClass::sound:
    case DeviceClass::vibration:
      return mState ? F("detected") : F("clear");

    case DeviceClass::presence:
      return mState ? F("home") : F("away");

    case DeviceClass::problem:
      return mState ? F("problem") : F("OK");

    case DeviceClass::safety:
      return mState ? F("unsafe") : F("safe");

    default:
      return mState ? F("on") : F("off");
  }
}

uint8_t BinarySensor::getDiscoveryMsg(uint8_t* buffer) {
  buffer[0] = getEntityId();
  buffer[1] = static_cast<uint8_t>(getComponent());
  buffer[2] = static_cast<uint8_t>(getDeviceClass());
  buffer[3] = 0;
  buffer[4] = 0;
  return 5;
}

void BinarySensor::print(Stream& stream) {
  stream.print(getName());
  stream.print(": ");
  stream.print(getStateName());
}
