/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include "Component.h"
#include "Stream.h"

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

  BinarySensor(uint8_t entityId, const char* name) :
      Component(entityId, name) {
  }

  virtual ~BinarySensor() = default;

  virtual bool update() = 0;

  virtual bool getState() const {
    return mState;
  }

  const char* getStateName() const {
    switch (getDeviceClass()) {
      case DeviceClass::Battery:
        return mState ? "low" : "normal";

      case DeviceClass::Cold:
        return mState ? "cold" : "normal";

      case DeviceClass::Heat:
        return mState ? "hot" : "normal";

      case DeviceClass::Connectivity:
        return mState ? "connected" : "disconnected";

      case DeviceClass::Door:
      case DeviceClass::GarageDoor:
      case DeviceClass::Opening:
      case DeviceClass::Window:
        return mState ? "open" : "closed";

      case DeviceClass::Lock:
        return mState ? "unlocked" : "locked";

      case DeviceClass::Moisture:
        return mState ? "wet" : "dry";

      case DeviceClass::Gas:
      case DeviceClass::Motion:
      case DeviceClass::Occupancy:
      case DeviceClass::Smoke:
      case DeviceClass::Sound:
      case DeviceClass::Vibration:
        return mState ? "detected" : "clear";

      case DeviceClass::Presence:
        return mState ? "home" : "away";

      case DeviceClass::Problem:
        return mState ? "problem" : "OK";

      case DeviceClass::Safety:
        return mState ? "unsafe" : "safe";

      default:
        return mState ? "on" : "off";
    }
  }

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
    buffer[4] = 0;
    return buffer;
  }

  void print(Stream& stream) {
    stream.print(mName);
    stream.print(": ");
    stream.print(getStateName());
//        BinarySensorStateName[static_cast<uint8_t>(getDeviceClass())][mState]);
  }

protected:
  bool mState = {};
};
