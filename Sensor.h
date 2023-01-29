#pragma once

#include <Stream.h>

#include "Component.h"
#include "Unit.h"
#include "Util.h"

enum class SensorDeviceClass {
  none,
  apparent_power,
  aqi,
  atmospheric_pressure,
  battery,
  carbon_dioxide,
  carbon_monoxide,
  current,
  data_rate,
  data_size,
  date,
  distance,
  duration,
  energy,
  enum_class,
  frequency,
  gas,
  humidity,
  illuminance,
  irradiance,
  moisture,
  monetary,
  nitrogen_dioxide,
  nitrogen_monoxide,
  nitrous_oxide,
  ozone,
  pm1,
  pm10,
  pm25,
  power_factor,
  power,
  precipitation,
  precipitation_intensity,
  pressure,
  reactive_power,
  signal_strength,
  sound_pressure,
  speed,
  sulphur_dioxide,
  temperature,
  timestamp,
  volatile_organic_compounds,
  voltage,
  volume,
  water,
  weight,
  wind_speed
};

template <class T>
class ISensor : public virtual IComponent {
 public:
  // From https://www.home-assistant.io/integrations/sensor/ at 2023-01-17

 protected:
  virtual ~ISensor() = default;

  virtual T getValue() const = 0;

  virtual Component::Type getComponentType() const = 0;

  virtual SensorDeviceClass getDeviceClass() const = 0;

  virtual Unit::Type getUnitType() const = 0;

  virtual const char* getUnitName() const = 0;

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) = 0;

  virtual uint8_t getValueMsg(uint8_t* buffer) = 0;

  virtual T absDiffLastReportedValue() const = 0;

  virtual void setValue(T value) = 0;
};

template <class T>
class Sensor : public virtual ISensor<T>, public Component {
 public:
  explicit Sensor(uint8_t entityId) : Component(entityId) {}

  Sensor(uint8_t entityId, const char* name,
         SensorDeviceClass deviceClass = SensorDeviceClass::none,
         Unit::Type unitType = Unit::Type::none, uint8_t precision = 0)
      : Component(entityId, name),
        mDeviceClass{deviceClass},
        mUnit{unitType},
        mPrecision{precision > 3 ? static_cast<uint8_t>(3) : precision},
        mScaleFactor{factors[mPrecision]} {}

  bool hasService() final { return false; }

  void callService(uint8_t service) final { (void)service; }

  T getValue() const final { return mValue; }

  Component::Type getComponentType() const final {
    return Component::Type::sensor;
  }

  SensorDeviceClass getDeviceClass() const final { return mDeviceClass; }

  Unit::Type getUnitType() const final { return mUnit.getType(); }

  const char* getUnitName() const final { return mUnit.getName(); }

  uint8_t getDiscoveryMsg(uint8_t* buffer) override {
    buffer[0] = getEntityId();
    buffer[1] = static_cast<uint8_t>(getComponentType());
    buffer[2] = static_cast<uint8_t>(getDeviceClass());
    buffer[3] = static_cast<uint8_t>(mUnit.getType());
    buffer[4] = (sizeof(T) << 4) | mPrecision;
    return 5;
  }

  uint8_t getValueMsg(uint8_t* buffer) final {
    uint8_t* p = buffer;
    *p++ = getEntityId();

    T* vp = reinterpret_cast<T*>(p);
    *vp = hton(getValue());
    p += sizeof(T);

    return p - buffer;
  }

  void setReported() final {
    Component::setReported();
    mLastReportedValue = mValue;
  }

  T absDiffLastReportedValue() const final {
    return abs(mValue - mLastReportedValue);
  }

  void print(Stream& stream) final {
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

  void print(Stream& stream, uint8_t service) final {
    (void)stream;
    (void)service;
  }

  void setValue(T value) final { mValue = value; }

 private:
  int16_t factors[4] = {1, 10, 100, 1000};

  T mValue = {};
  T mLastReportedValue = {};
  const SensorDeviceClass mDeviceClass = {SensorDeviceClass::none};
  const Unit mUnit = {Unit::Type::none};
  const uint8_t mPrecision = {};
  const int16_t mScaleFactor = {factors[mPrecision]};
};
