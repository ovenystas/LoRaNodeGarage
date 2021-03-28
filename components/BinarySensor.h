/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include "Component.h"

/*
 *
 */
class BinarySensor: public Component {
public:
  // From https://www.home-assistant.io/integrations/binary_sensor/ at 2021-03-21
  enum class DeviceClass {
    None,
    Battery,
    BatteryCharging,
    Cold,
    Connectivity,
    Door,
    GarageDoor,
    Gas,
    Heat,
    Light,
    Lock,
    Moisture,
    Motion,
    Moving,
    Occupancy,
    Opening,
    Plug,
    Power,
    Presence,
    Problem,
    Safety,
    Smoke,
    Sound,
    Vibration,
    Window
  };

  BinarySensor(uint8_t entityId) :
      Component(entityId) {
  }
  virtual ~BinarySensor() = default;

  virtual bool update() = 0;
  virtual bool getState() const { return mState; }

  Component::Type getComponent() const {
    return Component::Type::BinarySensor;
  }
  virtual DeviceClass getDeviceClass() const {
    return DeviceClass::None;
  }
  virtual uint8_t* getDiscoveryMsg(uint8_t* buffer) {
    buffer[0] = getEntityId();
    buffer[1] = static_cast<uint8_t>(getComponent());
    buffer[2] = static_cast<uint8_t>(getDeviceClass());
    buffer[3] = 0;
    return buffer;
  }

protected:
  bool mState = {};
};
