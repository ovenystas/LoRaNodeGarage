#include "Cover.h"

#include "Component.h"
#include "Unit.h"

uint8_t Cover::getDiscoveryMsg(uint8_t* buffer) {
  buffer[0] = getEntityId();
  buffer[1] = static_cast<uint8_t>(getComponentType());
  buffer[2] = static_cast<uint8_t>(getDeviceClass());
  buffer[3] = static_cast<uint8_t>(Unit::Type::none);
  buffer[4] = (1 << 4) | 0;
  return 5;
}

uint8_t Cover::getValueMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  *p++ = getEntityId();
  *p++ = static_cast<uint8_t>(getState());

  return p - buffer;
}

void Cover::print(Stream& stream) {
  stream.print(getName());
  stream.print(": ");
  stream.print(CoverStateName[static_cast<uint8_t>(mState)]);
}

void Cover::print(Stream& stream, uint8_t service) {
  stream.print(getName());
  stream.print(F(": Service "));
  stream.print(getServiceName(static_cast<CoverService>(service)));
  stream.print(F(" called when in state "));
  stream.print(CoverStateName[static_cast<uint8_t>(mState)]);
}

CoverService Cover::serviceDecode(uint8_t service) {
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
