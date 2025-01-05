#pragma once

#include <stdint.h>

#include "Component.h"
#include "ConfigItem.h"
#include "DistanceSensor.h"
#include "EeAdressMap.h"
#include "Sensor.h"
#include "Unit.h"
#include "Util.h"

using HeightT = int16_t;  // cm

namespace HeightSensorConstants {
static const uint16_t CONFIG_REPORT_HYSTERESIS_DEFAULT = 10;
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
static const uint16_t CONFIG_STABLE_TIME_DEFAULT = 5000;
static const HeightT CONFIG_ZERO_VALUE_DEFAULT = 60;
}  // namespace HeightSensorConstants

class HeightSensor : public IComponent {
 public:
  HeightSensor() = delete;

  HeightSensor(uint8_t entityId, const char* name,
               Sensor<DistanceT>& distanceSensor)
      : mSensor{Sensor<HeightT>(entityId, name, SensorDeviceClass::distance,
                                Unit::Type::cm)},
        mDistanceSensor{distanceSensor} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigItemValues(ConfigItemValueT* items,
                              uint8_t length) const final;

  void getDiscoveryItem(DiscoveryItemT* item) const final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  Sensor<HeightT>& getSensor() { return mSensor; }

  void getValueItem(ValueItemT* item) const final {
    return mSensor.getValueItem(item);
  }

  bool isReportDue() const final { return mSensor.isReportDue(); }

  void loadConfigValues() final;

  size_t printTo(Print& p) const final { return mSensor.printTo(p); };

  size_t printTo(Print& p, uint8_t service) const final {
    (void)p;
    (void)service;
    return 0;
  };

  bool setConfigItemValues(const ConfigItemValueT* items, uint8_t length) final;

  void setReported() final { mSensor.setReported(); }

  bool update() final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {4};

    ConfigItem<uint16_t> reportHysteresis = {ConfigItem<uint16_t>(
        0, EE_ADDRESS_CONFIG_HEIGHT_SENSOR_0,
        HeightSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT, 0,
        MAX_SENSOR_DISTANCE, Unit::Type::cm)};

    ConfigItem<uint16_t> reportInterval = {ConfigItem<uint16_t>(
        1, EE_ADDRESS_CONFIG_HEIGHT_SENSOR_1,
        HeightSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT, 0,
        Util::ONE_HOUR_IN_SECONDS, Unit::Type::s)};

    ConfigItem<uint16_t> stableTime = {ConfigItem<uint16_t>(
        2, EE_ADDRESS_CONFIG_HEIGHT_SENSOR_2,
        HeightSensorConstants::CONFIG_STABLE_TIME_DEFAULT, 0,
        Util::ONE_MINUTE_IN_MILLISECONDS, Unit::Type::ms)};

    ConfigItem<HeightT> zeroValue = {ConfigItem<HeightT>(
        3, EE_ADDRESS_CONFIG_HEIGHT_SENSOR_3,
        HeightSensorConstants::CONFIG_ZERO_VALUE_DEFAULT, -MAX_SENSOR_DISTANCE,
        MAX_SENSOR_DISTANCE, Unit::Type::cm)};
  };

  Sensor<HeightT> mSensor;
  Config mConfig;
  Sensor<DistanceT>& mDistanceSensor;
};
