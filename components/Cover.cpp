/*
 *  Created on: 13 apr. 2021
 *      Author: oveny
 */

#include "Cover.h"
#include "Component.h"

uint8_t* Cover::getDiscoveryMsg(uint8_t* buffer) {
  buffer[0] = mEntityId;
  buffer[1] = static_cast<uint8_t>(getComponent());
  buffer[2] = static_cast<uint8_t>(getDeviceClass());
  buffer[3] = 0;
  buffer[4] = 0;
  return buffer;
}

void Cover::print(Stream& stream) {
  stream.print(mName);
  stream.print(": ");
  stream.print(CoverStateName[mState]);
}
