/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include "Component.h"
#include "Unit.h"
#include "Util.h"

/*
 *
 */
template<class T>
class Sensor: public Component {
public:
  // From https://www.home-assistant.io/integrations/sensor/ at 2021-03-21
  enum class DeviceClass {
    None,
    Battery,
    Current,
    Energy,
    Humidity,
    Illuminance,
    SignalStrength,
    Temperature,
    Power,
    PowerFactor,
    Pressure,
    Timestamp,
    Voltage
  };

  Sensor(uint8_t entityId) :
      Component(entityId) {
  }
  virtual ~Sensor() = default;

  virtual bool update() = 0;
  virtual T getValue() const {
    return mValue;
  }
  void setReported() {
    mLastReportTime = seconds();
    mLastReportedValue = mValue;
  }
  Component::Type getComponent() const {
    return Component::Type::Sensor;
  }
  virtual DeviceClass getDeviceClass() const {
    return DeviceClass::None;
  }
  virtual Unit getUnit() const {
    return Unit::None;
  }
  virtual uint8_t* getDiscoveryMsg(uint8_t* buffer) {
    buffer[0] = getEntityId();
    buffer[1] = static_cast<uint8_t>(getComponent());
    buffer[2] = static_cast<uint8_t>(getDeviceClass());
    buffer[3] = 0;
    return buffer;
  }

protected:
  T mValue = { };
  T mLastReportedValue = { };
  uint32_t mLastReportTime = { }; // s
};
