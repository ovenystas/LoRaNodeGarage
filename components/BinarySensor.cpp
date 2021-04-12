/*
 *  Created on: 13 apr. 2021
 *      Author: oveny
 */

#include "BinarySensor.h"
#include "Component.h"

const __FlashStringHelper* BinarySensor::getStateName() {
  switch (getDeviceClass()) {
    case DeviceClass::Battery:
      return mState ? F("low") : F("normal");

    case DeviceClass::Cold:
      return mState ? F("cold") : F("normal");

    case DeviceClass::Heat:
      return mState ? F("hot") : F("normal");

    case DeviceClass::Connectivity:
      return mState ? F("connected") : F("disconnected");

    case DeviceClass::Door:
    case DeviceClass::GarageDoor:
    case DeviceClass::Opening:
    case DeviceClass::Window:
      return mState ? F("open") : F("closed");

    case DeviceClass::Lock:
      return mState ? F("unlocked") : F("locked");

    case DeviceClass::Moisture:
      return mState ? F("wet") : F("dry");

    case DeviceClass::Gas:
    case DeviceClass::Motion:
    case DeviceClass::Occupancy:
    case DeviceClass::Smoke:
    case DeviceClass::Sound:
    case DeviceClass::Vibration:
      return mState ? F("detected") : F("clear");

    case DeviceClass::Presence:
      return mState ? F("home") : F("away");

    case DeviceClass::Problem:
      return mState ? F("problem") : F("OK");

    case DeviceClass::Safety:
      return mState ? F("unsafe") : F("safe");

    default:
      return mState ? F("on") : F("off");
  }
}

uint8_t* BinarySensor::getDiscoveryMsg(uint8_t* buffer) {
  buffer[0] = mEntityId;
  buffer[1] = static_cast<uint8_t>(getComponent());
  buffer[2] = static_cast<uint8_t>(getDeviceClass());
  buffer[3] = 0;
  buffer[4] = 0;
  return buffer;
}

void BinarySensor::print(Stream& stream) {
  stream.print(mName);
  stream.print(": ");
  stream.print(getStateName());
}
