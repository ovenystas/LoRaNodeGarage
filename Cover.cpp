#include "Cover.h"

#include "BaseComponent.h"
#include "Unit.h"

static const char CoverStateName[][8] = {"closed", "open", "opening",
                                         "closing"};

static const char CoverServiceName[][8] = {"open", "close", "stop", "toggle",
                                           "unknown"};

const char* Cover::getServiceName(CoverService service) const {
  return CoverServiceName[static_cast<uint8_t>(service)];
}

void Cover::getDiscoveryEntityItem(DiscoveryEntityItemT* item) const {
  item->entityId = mBaseComponent.getEntityId();
  item->componentType = static_cast<uint8_t>(getComponentType());
  item->deviceClass = static_cast<uint8_t>(getDeviceClass());
  item->unit = static_cast<uint8_t>(Unit::Type::none);
  item->isSigned = false;
  item->sizeCode = 0;
  item->precision = 0;
}

const char* Cover::getStateName() const {
  return CoverStateName[static_cast<uint8_t>(mState)];
}

const char* Cover::getStateName(CoverState state) const {
  return CoverStateName[static_cast<uint8_t>(state)];
}

void Cover::getValueItem(ValueItemT* item) const {
  item->entityId = mBaseComponent.getEntityId();
  item->value = static_cast<uint32_t>(mState);
}

size_t Cover::print(Print& printer) const {
  size_t n = 0;
  n += printer.print(mBaseComponent.getName());
  n += printer.print(": ");
  n += printer.print(CoverStateName[static_cast<uint8_t>(mState)]);
  return n;
}

size_t Cover::print(Print& printer, uint8_t service) const {
  size_t n = 0;
  n += printer.print(mBaseComponent.getName());
  n += printer.print(F(": Service "));
  n += printer.print(getServiceName(static_cast<CoverService>(service)));
  n += printer.print(F(" called when in state "));
  n += printer.print(CoverStateName[static_cast<uint8_t>(mState)]);
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
