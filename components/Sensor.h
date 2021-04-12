/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include "Component.h"
#include "Unit.h"
#include "Util.h"

#include <Stream.h>

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

  Sensor(uint8_t entityId, const char* name) :
      Component(entityId, name) {
  }

  Sensor(uint8_t entityId, const char* name, Unit::TypeE unitType) :
      Component(entityId, name), mUnit(unitType) {
  }

  Sensor(uint8_t entityId, const char* name, Unit::TypeE unitType,
      uint8_t precision) :
      Component(entityId, name), mUnit(unitType) {

    mPrecision = precision > 3 ? 3: precision;

    int16_t factors[4] = { 1, 10, 100, 1000 };
    mScaleFactor = factors[mPrecision];
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

  virtual Unit::TypeE getUnitType() const {
    return mUnit.getType();
  }

  virtual const String& getUnitName() const {
    return mUnit.getName();
  }

  virtual uint8_t* getDiscoveryMsg(uint8_t* buffer) {
    buffer[0] = mEntityId;
    buffer[1] = static_cast<uint8_t>(getComponent());
    buffer[2] = static_cast<uint8_t>(getDeviceClass());
    buffer[3] = static_cast<uint8_t>(mUnit.getType());
    buffer[4] = mPrecision;
    return buffer;
  }

  void print(Stream& stream) {
    stream.print(mName);
    stream.print(": ");
    if (mScaleFactor == 1) {
      stream.print(mValue);
    }
    else {
      stream.print(mValue / mScaleFactor);
      stream.print('.');
      stream.print(abs(mValue % mScaleFactor));
    }
    stream.print(' ');
    stream.print(mUnit.getName());
  }

protected:
  T mValue = { };
  T mLastReportedValue = { };
  uint32_t mLastReportTime = { }; // s
  Unit mUnit = { Unit::TypeE::None };
  uint8_t mPrecision = { };
  int16_t mScaleFactor = { 1 };
};
