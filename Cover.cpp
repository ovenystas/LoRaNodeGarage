#include "Cover.h"

#include "BaseComponent.h"
#include "Unit.h"

const __FlashStringHelper* Cover::getStateName() const {
  switch(mState) {
    case CoverState::closed:
      return F("closed");
    
    case CoverState::open:
      return F("open");

    case CoverState::opening:
      return F("opening");

    case CoverState::closing:
      return F("closing");

    default:
      return F("unkown");
  }
}

const __FlashStringHelper* Cover::getServiceName(CoverService service) const {
  switch(service) {
    case CoverService::open:
      return F("open");
    
    case CoverService::close:
      return F("close");

    case CoverService::stop:
      return F("stop");

    case CoverService::toggle:
      return F("toggle");

    case CoverService::unknown:
    default:
      return F("unkown");
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
