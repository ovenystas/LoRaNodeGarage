#pragma once

#include <Printable.h>

#include "BaseComponent.h"
#include "Types.h"
#include "Unit.h"
#include "Util.h"
#include "ValueItem.h"

// From https://www.home-assistant.io/integrations/sensor/ at 2023-01-17
enum class SensorDeviceClass : uint8_t {
  NONE,
  APPARENT_POWER,
  AQI,
  ATMOSPHERIC_PRESSURE,
  BATTERY,
  CARBON_DIOXIDE,
  CARBON_MONOXIDE,
  CURRENT,
  DATA_RATE,
  DATA_SIZE,
  DATE,
  DISTANCE,
  DURATION,
  ENERGY,
  ENUM_CLASS,
  FREQUENCY,
  GAS,
  HUMIDITY,
  ILLUMINANCE,
  IRRADIANCE,
  MOISTURE,
  MONETARY,
  NITROGEN_DIOXIDE,
  NITROGEN_MONOXIDE,
  NITROUS_OXIDE,
  OZONE,
  PM1,
  PM10,
  PM25,
  POWER_FACTOR,
  POWER,
  PRECIPITATION,
  PRECIPITATION_INTENSITY,
  PRESSURE,
  REACTIVE_POWER,
  SIGNAL_STRENGTH,
  SOUND_PRESSURE,
  SPEED,
  SULPHUR_DIOXIDE,
  TEMPERATURE,
  TIMESTAMP,
  VOLATILE_ORGANIC_COMPOUNDS,
  VOLTAGE,
  VOLUME,
  WATER,
  WEIGHT,
  WIND_SPEED
};

template <class T>
class Sensor : public Printable {
 public:
  Sensor() = delete;

  explicit Sensor(uint8_t entityId) : mBaseComponent{BaseComponent(entityId)} {}

  Sensor(uint8_t entityId, const char* name,
         SensorDeviceClass deviceClass = SensorDeviceClass::NONE,
         Unit::Type unitType = Unit::Type::none, uint8_t precision = 0)
      : mBaseComponent{BaseComponent(entityId, name, BaseComponent::Category::DIAGNOSTIC)},
        mDeviceClass{deviceClass},
        mValueItem{ValueItem<T>(unitType, precision)} {}

  BaseComponent::Type getComponentType() const {
    return BaseComponent::Type::SENSOR;
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

  Unit::Type getUnitType() const { return mValueItem.getUnit().type(); }

  T getValue() const { return mValueItem.getValue(); }

  void getValueItem(ValueItemT* item) const {
    item->entityId = mBaseComponent.getEntityId();
    item->value = static_cast<uint32_t>(mValueItem.getValue());
  }

  bool isReportDue() const { return mBaseComponent.isReportDue(); }

  size_t printTo(Print& p) const final {
    size_t n = 0;
    n += mBaseComponent.printTo(p);
    n += p.print('=');
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
  const SensorDeviceClass mDeviceClass{SensorDeviceClass::NONE};
  ValueItem<T> mValueItem;
  T mLastReportedValue{};
};
