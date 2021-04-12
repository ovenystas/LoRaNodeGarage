/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include "Component.h"
#include "Stream.h"

static const String BinarySensorStateName[] = { "off", "on" };

//static const String BinarySensorStateName[25][2] = {
//    { "off", "on" },
//    { "normal", "low" },
//    { "off", "on" },
//    { "normal", "cold" },
//    { "disconnected", "connected" },
//    { "closed", "open" },
//    { "closed", "open" },
//    { "clear", "detected" },
//    { "normal", "hot" },
//    { "off", "on" },
//    { "locked", "unlocked" },
//    { "dry", "wet" },
//    { "clear", "detected" },
//    { "off", "on" },
//    { "clear", "detected" },
//    { "closed", "open" },
//    { "off", "on" },
//    { "off", "on" },
//    { "away", "home" },
//    { "OK", "problem" },
//    { "safe", "unsafe" },
//    { "clear", "detected" },
//    { "clear", "detected" },
//    { "clear", "detected" },
//    { "closed", "open" },
//};

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
    buffer[4] = 0;
    return buffer;
  }

  void print(Stream& stream) {
    stream.print(mName);
    stream.print(": ");
    stream.print(BinarySensorStateName[mState]);
//        BinarySensorStateName[static_cast<uint8_t>(getDeviceClass())][mState]);
  }

protected:
  bool mState = {};
};
