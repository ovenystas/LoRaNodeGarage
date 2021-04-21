/*
 *  Created on: 21 mars 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include <WString.h>

#define COMPONENT_NAME_LENGTH_MAX 20

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

    strncpy(mName, name, COMPONENT_NAME_LENGTH_MAX);
    mName[COMPONENT_NAME_LENGTH_MAX - 1] = '\0';
  }

  virtual ~Component() = default;

  inline uint8_t getEntityId() const {
    return mEntityId;
  }

  virtual uint8_t* getDiscoveryMsg(uint8_t* buffer) = 0;

  inline const char* getName() const {
    return mName;
  }

private:
  const uint8_t mEntityId = { };
  char mName[COMPONENT_NAME_LENGTH_MAX] = { };
};
