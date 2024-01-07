#pragma once

#include <Printable.h>

#include "BaseComponent.h"
#include "Types.h"
#include "Unit.h"
#include "Util.h"
#include "ValueItem.h"

// From https://www.home-assistant.io/integrations/sensor/ at 2023-01-17
enum class SensorDeviceClass : uint8_t {
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
class Sensor : public Printable {
 public:
  Sensor() = delete;

  explicit Sensor(uint8_t entityId) : mBaseComponent{BaseComponent(entityId)} {}

  Sensor(uint8_t entityId, const char* name,
         SensorDeviceClass deviceClass = SensorDeviceClass::none,
         Unit::Type unitType = Unit::Type::none, uint8_t precision = 0)
      : mBaseComponent{BaseComponent(entityId, name)},
        mDeviceClass{deviceClass},
        mValueItem{ValueItem<T>(unitType, precision)} {}

  BaseComponent::Type getComponentType() const {
    return BaseComponent::Type::sensor;
  }

  SensorDeviceClass getDeviceClass() const { return mDeviceClass; }

  void getDiscoveryEntityItem(DiscoveryEntityItemT* item) const {
    item->entityId = mBaseComponent.getEntityId();
    item->componentType = static_cast<uint8_t>(getComponentType());
    item->deviceClass = static_cast<uint8_t>(getDeviceClass());
    item->unit = static_cast<uint8_t>(mValueItem.getUnit().type());
    item->isSigned = mValueItem.isSigned();
    item->sizeCode = static_cast<uint8_t>(mValueItem.getValueSize()) / 2;
    item->precision = mValueItem.getPrecision();
  }

  uint8_t getEntityId() const { return mBaseComponent.getEntityId(); }

  T absDiffLastReportedValue() const {
    return abs(mValueItem.getValue() - mLastReportedValue);
  }

  const char* getUnitName() const { return mValueItem.getUnit().name(); }

  Unit::Type getUnitType() const { return mValueItem.getUnit().type(); }

  T getValue() const { return mValueItem.getValue(); }

  void getValueItem(ValueItemT* item) const {
    item->entityId = mBaseComponent.getEntityId();
    item->value = static_cast<uint32_t>(mValueItem.getValue());
  }

  bool isReportDue() const { return mBaseComponent.isReportDue(); }

  size_t printTo(Print& p) const final {
    size_t n = 0;
    n += p.print(mBaseComponent.getName());
    n += p.print(": ");
    n += mValueItem.printTo(p);
    return n;
  }

  void setIsReportDue(bool isDue) { mBaseComponent.setIsReportDue(isDue); }

  void setReported() {
    mBaseComponent.setReported();
    mLastReportedValue = mValueItem.getValue();
  }

  void setValue(T value) { mValueItem.setValue(value); }

  uint32_t timeSinceLastReport() const {
    return mBaseComponent.timeSinceLastReport();
  }

 private:
  BaseComponent mBaseComponent;
  const SensorDeviceClass mDeviceClass{SensorDeviceClass::none};
  ValueItem<T> mValueItem;
  T mLastReportedValue{};
};
