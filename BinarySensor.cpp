#include "BinarySensor.h"

#include <WString.h>

#include "BaseComponent.h"
#include "Unit.h"

const __FlashStringHelper* BinarySensor::getStateName() const {
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

void BinarySensor::getDiscoveryEntityItem(DiscoveryEntityItemT* item) const {
  item->entityId = mBaseComponent.getEntityId();
  item->componentType = static_cast<uint8_t>(getComponentType());
  item->deviceClass = static_cast<uint8_t>(getDeviceClass());
  item->unit = static_cast<uint8_t>(mUnit.type());
  item->isSigned = false;
  item->sizeCode = 0;
  item->precision = 0;
}

void BinarySensor::getValueItem(ValueItemT* item) const {
  item->entityId = mBaseComponent.getEntityId();
  item->value = static_cast<uint32_t>(mState);
}

size_t BinarySensor::print(Print& printer) const {
  size_t n = 0;
  n += printer.print(mBaseComponent.getName());
  n += printer.print(": ");
  n += printer.print(getStateName());
  return n;
}
