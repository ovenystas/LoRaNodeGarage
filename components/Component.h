/*
 *  Created on: 21 mars 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>

class Component {
public:
  enum class Type {
    BinarySensor,
    Sensor,
    Cover
  };

  Component(uint8_t entityId) :
      mEntityId { entityId } {
  }
  virtual ~Component() = default;
  virtual uint8_t getEntityId() const {
    return mEntityId;
  }
  virtual uint8_t* getDiscoveryMsg(uint8_t* buffer) = 0;

protected:
  const uint8_t mEntityId = { };
};
