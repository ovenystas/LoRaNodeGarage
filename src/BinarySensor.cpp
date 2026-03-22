#include "BinarySensor.h"

#include <WString.h>

#include "BaseComponent.h"
#include "Unit.h"

const __FlashStringHelper* BinarySensor::getStateName() const {
  switch (getDeviceClass()) {
    case BinarySensorDeviceClass::BATTERY:
      return mState ? F("low") : F("normal");

    case BinarySensorDeviceClass::COLD:
      return mState ? F("cold") : F("normal");

    case BinarySensorDeviceClass::HEAT:
      return mState ? F("hot") : F("normal");

    case BinarySensorDeviceClass::CONNECTIVITY:
      return mState ? F("connected") : F("disconnected");

    case BinarySensorDeviceClass::DOOR:
    case BinarySensorDeviceClass::GARAGE_DOOR:
    case BinarySensorDeviceClass::OPENING:
    case BinarySensorDeviceClass::WINDOW:
      return mState ? F("open") : F("closed");

    case BinarySensorDeviceClass::LOCK:
      return mState ? F("unlocked") : F("locked");

    case BinarySensorDeviceClass::MOISTURE:
      return mState ? F("wet") : F("dry");

    case BinarySensorDeviceClass::GAS:
    case BinarySensorDeviceClass::MOTION:
    case BinarySensorDeviceClass::OCCUPANCY:
    case BinarySensorDeviceClass::SMOKE:
    case BinarySensorDeviceClass::SOUND:
    case BinarySensorDeviceClass::VIBRATION:
      return mState ? F("detected") : F("clear");

    case BinarySensorDeviceClass::PRESENCE:
      return mState ? F("home") : F("away");

    case BinarySensorDeviceClass::PROBLEM:
      return mState ? F("problem") : F("OK");

    case BinarySensorDeviceClass::SAFETY:
      return mState ? F("unsafe") : F("safe");

    default:
      return mState ? F("on") : F("off");
  }
}

void BinarySensor::getDiscoveryEntityItem(DiscoveryEntityItemT* item) const {
  item->entityId = mBaseComponent.getEntityId();
  item->componentType = static_cast<uint8_t>(getComponentType());
  item->deviceClass = static_cast<uint8_t>(getDeviceClass());
  item->category = static_cast<uint8_t>(getCategory());
  item->unit = static_cast<uint8_t>(mUnit.type());
  item->isSigned = false;
  item->sizeCode = 0;
  item->precision = 0;
  item->minValue = 0;
  item->maxValue = 1;
  strncpy(item->name, mBaseComponent.getName(), sizeof(item->name));
  item->name[sizeof(item->name) - 1] = '\0'; // Ensure null termination
}

void BinarySensor::getValueItem(ValueItemT* item) const {
  item->entityId = mBaseComponent.getEntityId();
  item->value = static_cast<uint32_t>(mState);
}

size_t BinarySensor::printTo(Print& p) const {
  size_t n = 0;
  n += mBaseComponent.printTo(p);
  n += p.print('=');
  n += p.print(getStateName());
  return n;
}
