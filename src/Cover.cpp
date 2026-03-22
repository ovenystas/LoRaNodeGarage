#include "Cover.h"

#include "BaseComponent.h"
#include "Unit.h"

const __FlashStringHelper* Cover::getStateName() const {
  switch (mState) {
    case CoverState::CLOSED:
      return F("closed");

    case CoverState::OPEN:
      return F("open");

    case CoverState::OPENING:
      return F("opening");

    case CoverState::CLOSING:
      return F("closing");

    default:
      return F("unknown");
  }
}

const __FlashStringHelper* Cover::getServiceName(CoverService service) const {
  switch (service) {
    case CoverService::OPEN:
      return F("open");

    case CoverService::CLOSE:
      return F("close");

    case CoverService::STOP:
      return F("stop");

    case CoverService::TOGGLE:
      return F("toggle");

    case CoverService::UNKNOWN:
    default:
      return F("unknown");
  }
}

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
  n += p.print('=');
  n += p.print(getStateName());
  return n;
}

size_t Cover::printTo(Print& p, uint8_t service) const {
  size_t n = 0;
  n += mBaseComponent.printTo(p);
  n += p.print(F(": service="));
  n += p.print(getServiceName(serviceDecode(service)));
  n += p.print(F(" state="));
  n += p.print(getStateName());
  return n;
}

CoverService Cover::serviceDecode(uint8_t service) const {
  switch (service) {
    case 0:
      return CoverService::OPEN;
    case 1:
      return CoverService::CLOSE;
    case 2:
      return CoverService::STOP;
    case 3:
      return CoverService::TOGGLE;
    default:
      return CoverService::UNKNOWN;
  }
}
