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

uint8_t Cover::getDiscoveryMsg(uint8_t* buffer) {
  buffer[0] = mBaseComponent.getEntityId();
  buffer[1] = static_cast<uint8_t>(getComponentType());
  buffer[2] = static_cast<uint8_t>(getDeviceClass());
  buffer[3] = static_cast<uint8_t>(Unit::Type::none);
  buffer[4] = (1 << 4) | 0;
  return 5;
}

const char* Cover::getStateName() const {
  return CoverStateName[static_cast<uint8_t>(mState)];
}

const char* Cover::getStateName(CoverState state) const {
  return CoverStateName[static_cast<uint8_t>(state)];
}

uint8_t Cover::getValueMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  *p++ = mBaseComponent.getEntityId();
  *p++ = static_cast<uint8_t>(getState());

  return p - buffer;
}

size_t Cover::print(Stream& stream) {
  size_t n = 0;
  n += stream.print(mBaseComponent.getName());
  n += stream.print(": ");
  n += stream.print(CoverStateName[static_cast<uint8_t>(mState)]);
  return n;
}

size_t Cover::print(Stream& stream, uint8_t service) {
  size_t n = 0;
  n += stream.print(mBaseComponent.getName());
  n += stream.print(F(": Service "));
  n += stream.print(getServiceName(static_cast<CoverService>(service)));
  n += stream.print(F(" called when in state "));
  n += stream.print(CoverStateName[static_cast<uint8_t>(mState)]);
  return n;
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
