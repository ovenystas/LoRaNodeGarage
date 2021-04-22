/*
 *  Created on: 21 mars 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include <WString.h>

class Component {
public:
  enum class Type {
    binarySensor,
    sensor,
    cover
  };

  Component(uint8_t entityId) :
      mEntityId { entityId } {
  }

  Component(uint8_t entityId, const char* name) :
      mEntityId { entityId } {

    strncpy(mName, name, maxNameLength);
    mName[maxNameLength - 1] = '\0';
  }

  virtual ~Component() = default;

  inline uint8_t getEntityId() const {
    return mEntityId;
  }

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) = 0;

  inline const char* getName() const {
    return mName;
  }

private:
  static const uint8_t maxNameLength = 20;

  const uint8_t mEntityId = { };
  char mName[maxNameLength] = { };
};
