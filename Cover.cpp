#include "Cover.h"

#include "BaseComponent.h"
#include "Unit.h"

static const char CoverStateName[][8] PROGMEM = {"closed", "open", "opening",
                                                 "closing"};

static const char CoverServiceName[][8] PROGMEM = {"open", "close", "stop",
                                                   "toggle", "unknown"};

void Cover::getDiscoveryEntityItem(DiscoveryEntityItemT* item) const {
  item->entityId = mBaseComponent.getEntityId();
  item->componentType = static_cast<uint8_t>(getComponentType());
  item->deviceClass = static_cast<uint8_t>(getDeviceClass());
  item->unit = static_cast<uint8_t>(Unit::Type::none);
  item->isSigned = false;
  item->sizeCode = 0;
  item->precision = 0;
}

void Cover::getValueItem(ValueItemT* item) const {
  item->entityId = mBaseComponent.getEntityId();
  item->value = static_cast<uint32_t>(mState);
}

size_t Cover::printTo(Print& p) const {
  size_t n = 0;
  n += mBaseComponent.printTo(p);
  n += p.print(": ");
  n += p.print(CoverStateName[static_cast<uint8_t>(mState)]);
  return n;
}

size_t Cover::printTo(Print& p, uint8_t service) const {
  size_t n = 0;
  n += mBaseComponent.printTo(p);
  n += p.print(F(": Service "));
  n += p.print(reinterpret_cast<const __FlashStringHelper*>(
      CoverServiceName[static_cast<uint8_t>(service)]));
  n += p.print(F(" called when in state "));
  n += p.print(reinterpret_cast<const __FlashStringHelper*>(
      CoverStateName[static_cast<uint8_t>(mState)]));
  return n;
}

CoverService Cover::serviceDecode(uint8_t service) const {
  switch (service) {
    case 0:
      return CoverService::open;
    case 1:
      return CoverService::close;
    case 2:
      return CoverService::stop;
    case 3:
      return CoverService::toggle;
    default:
      return CoverService::unknown;
  }
}
