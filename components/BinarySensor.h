/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <Stream.h>

#include "Component.h"
#include "Util.h"

/*
 *
 */
class BinarySensor: public Component {
public:
  // From https://www.home-assistant.io/integrations/binary_sensor/ at 2021-03-21
  enum class DeviceClass {
    none,
    battery,
    batteryCharging,
    cold,
    connectivity,
    door,
    garageDoor,
    gas,
    heat,
    light,
    lock,
    moisture,
    motion,
    moving,
    occupancy,
    opening,
    plug,
    power,
    presence,
    problem,
    safety,
    smoke,
    sound,
    vibration,
    window
  };

  BinarySensor(uint8_t entityId, const char* name) :
      Component(entityId, name) {
  }

  virtual ~BinarySensor() = default;

  bool getState() const {
    return mState;
  }

  const __FlashStringHelper* getStateName();

  Component::Type getComponent() const {
    return Component::Type::binarySensor;
  }

  virtual DeviceClass getDeviceClass() const {
    return DeviceClass::none;
  }

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) override;

  virtual uint8_t getValueMsg(uint8_t* buffer) final;

  virtual void setReported() final {
    mLastReportTime = seconds();
    mLastReportedState = mState;
  }

  virtual void print(Stream& stream) final;

protected:
  inline void setState(bool state) {
    mState = state;
  }

private:
  bool mState = { };
  bool mLastReportedState = { };
  uint32_t mLastReportTime = { }; // s
};
