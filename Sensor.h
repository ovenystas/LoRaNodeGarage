#pragma once

#include <Stream.h>

#include "BaseComponent.h"
#include "Types.h"
#include "Unit.h"
#include "Util.h"

// From https://www.home-assistant.io/integrations/sensor/ at 2023-01-17
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
class Sensor {
 public:
  Sensor() = delete;

  explicit Sensor(uint8_t entityId) : mBaseComponent{BaseComponent(entityId)} {}

  Sensor(uint8_t entityId, const char* name,
         SensorDeviceClass deviceClass = SensorDeviceClass::none,
         Unit::Type unitType = Unit::Type::none, uint8_t precision = 0)
      : mBaseComponent{BaseComponent(entityId, name)},
        mDeviceClass{deviceClass},
        mUnit{unitType},
        mPrecision{precision > 3 ? static_cast<uint8_t>(3) : precision},
        mScaleFactor{factors[mPrecision]} {}

  BaseComponent::Type getComponentType() const {
    return BaseComponent::Type::sensor;
  }

  SensorDeviceClass getDeviceClass() const { return mDeviceClass; }

  void getDiscoveryEntityItem(DiscoveryEntityItemT* item) const {
    item->entityId = mBaseComponent.getEntityId();
    item->componentType = static_cast<uint8_t>(getComponentType());
    item->deviceClass = static_cast<uint8_t>(getDeviceClass());
    item->unit = static_cast<uint8_t>(mUnit.getType());
    item->size = sizeof(T);
    item->precision = mPrecision;
  }

  uint8_t getEntityId() const { return mBaseComponent.getEntityId(); }

  T absDiffLastReportedValue() const {
    return abs(mValue - mLastReportedValue);
  }

  const char* getUnitName() const { return mUnit.getName(); }

  Unit::Type getUnitType() const { return mUnit.getType(); }

  T getValue() const { return mValue; }

  void getValueItem(ValueItemT* item) const {
    item->entityId = mBaseComponent.getEntityId();
    item->value = static_cast<uint32_t>(mValue);
  }

  size_t print(Stream& stream) const {
    size_t n = 0;
    n += stream.print(mBaseComponent.getName());
    n += stream.print(": ");

    if (mScaleFactor == 1) {
      n += stream.print(mValue);
    } else {
      if (mValue < 0) {
        n += stream.print('-');
      }
      uint32_t integer = abs(mValue / mScaleFactor);
      n += stream.print(integer);

      n += stream.print('.');

      uint32_t fractional = abs(mValue % mScaleFactor);
      if (mScaleFactor >= 1000 && fractional < 100) {
        n += stream.print('0');
      }
      if (mScaleFactor >= 100 && fractional < 10) {
        n += stream.print('0');
      }
      n += stream.print(fractional);
    }

    if (mUnit.getType() != Unit::Type::none) {
      n += stream.print(' ');
      n += stream.print(mUnit.getName());
    }
    return n;
  }

  void setReported() {
    mBaseComponent.setReported();
    mLastReportedValue = mValue;
  }

  void setValue(T value) { mValue = value; }

  uint32_t timeSinceLastReport() const {
    return mBaseComponent.timeSinceLastReport();
  }

 private:
  const int16_t factors[4] = {1, 10, 100, 1000};

  BaseComponent mBaseComponent;
  T mValue{};
  T mLastReportedValue{};
  const SensorDeviceClass mDeviceClass = {SensorDeviceClass::none};
  const Unit mUnit{Unit::Type::none};
  const uint8_t mPrecision{};
  const int16_t mScaleFactor{factors[mPrecision]};
};
