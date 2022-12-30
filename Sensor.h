/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <Stream.h>

#include "Component.h"
#include "Unit.h"
#include "Util.h"

/*
 *
 */
template <class T>
class Sensor : public Component {
 public:
  // From https://www.home-assistant.io/integrations/sensor/ at 2021-03-21
  enum class DeviceClass {
    none,
    battery,
    current,
    energy,
    humidity,
    illuminance,
    signalStrength,
    temperature,
    power,
    powerFactor,
    pressure,
    timestamp,
    voltage
  };

  Sensor(uint8_t entityId) : Component(entityId) {}

  Sensor(uint8_t entityId, const char* name,
         Unit::Type unitType = Unit::Type::none, uint8_t precision = 0)
      : Component(entityId, name),
        mUnit{unitType},
        mPrecision{precision > 3 ? static_cast<uint8_t>(3) : precision},
        mScaleFactor{factors[mPrecision]} {}

  virtual ~Sensor() = default;

  virtual bool hasService() final { return false; }

  virtual void callService(uint8_t service) final { (void)service; }

  virtual void print(Stream& stream, uint8_t service) final {
    (void)stream;
    (void)service;
  }

  inline T getValue() const { return mValue; }

  virtual void setReported() final {
    mLastReportTime = seconds();
    mLastReportedValue = mValue;
  }

  Component::Type getComponent() const { return Component::Type::sensor; }

  virtual DeviceClass getDeviceClass() const { return DeviceClass::none; }

  inline Unit::Type getUnitType() const { return mUnit.getType(); }

  inline const char* getUnitName() const { return mUnit.getName(); }

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) override {
    buffer[0] = getEntityId();
    buffer[1] = static_cast<uint8_t>(getComponent());
    buffer[2] = static_cast<uint8_t>(getDeviceClass());
    buffer[3] = static_cast<uint8_t>(mUnit.getType());
    buffer[4] = (sizeof(T) << 4) | mPrecision;
    return 5;
  }

  virtual uint8_t getValueMsg(uint8_t* buffer) final {
    uint8_t* p = buffer;
    *p++ = getEntityId();

    T* vp = reinterpret_cast<T*>(p);
    *vp = hton(getValue());
    p += sizeof(T);

    return p - buffer;
  }

  virtual void print(Stream& stream) final {
    stream.print(getName());
    stream.print(": ");
    if (mScaleFactor == 1) {
      stream.print(mValue);
    } else {
      stream.print(mValue / mScaleFactor);
      stream.print('.');
      stream.print(abs(mValue % mScaleFactor));
    }
    stream.print(' ');
    stream.print(mUnit.getName());
  }

  uint32_t timeSinceLastReport() const { return seconds() - mLastReportTime; }

  T absDiffSinceReportedValue(T newValue) const {
    return abs(newValue - mLastReportedValue);
  }

 protected:
  inline void setValue(T value) { mValue = value; }

 private:
  static constexpr int16_t factors[4] = {1, 10, 100, 1000};

  T mValue = {};
  T mLastReportedValue = {};
  uint32_t mLastReportTime = {};  // s
  const Unit mUnit;
  const uint8_t mPrecision;
  const int16_t mScaleFactor;
};
