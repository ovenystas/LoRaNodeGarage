#pragma once

#include <NewPing.h>

#include "Component.h"
#include "ConfigItem.h"
#include "EeAdressMap.h"
#include "Sensor.h"
#include "Util.h"

using DistanceT = int16_t;  // cm

namespace DistanceSensorConstants {
static const DistanceT CONFIG_REPORT_HYSTERESIS_DEFAULT = 10;
static const uint16_t CONFIG_MEASURE_INTERVAL_DEFAULT = 60;
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
}  // namespace DistanceSensorConstants

class DistanceSensor : public IComponent {
 public:
  virtual ~DistanceSensor() = default;

  DistanceSensor() = delete;

  DistanceSensor(uint8_t entityId, const char* name, NewPing& sonar)
      : mSensor{Sensor<DistanceT>(entityId, name, SensorDeviceClass::distance,
                                  Unit::Type::cm)},
        mSonar{sonar} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigItemValues(ConfigItemValueT* items,
                              uint8_t length) const final;

  void getDiscoveryItem(DiscoveryItemT* item) const final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  Sensor<DistanceT>& getSensor() { return mSensor; }

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
    static const uint8_t numberOfConfigItems = {3};

    ConfigItem<DistanceT> reportHysteresis = {ConfigItem<DistanceT>(
        0, EE_ADDRESS_CONFIG_DISTANCESENSOR_0,
        DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT,
        Unit::Type::cm)};

    ConfigItem<uint16_t> measureInterval = {ConfigItem<uint16_t>(
        1, EE_ADDRESS_CONFIG_DISTANCESENSOR_1,
        DistanceSensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT,
        Unit::Type::s)};

    ConfigItem<uint16_t> reportInterval = {ConfigItem<uint16_t>(
        2, EE_ADDRESS_CONFIG_DISTANCESENSOR_2,
        DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT,
        Unit::Type::s)};
  };

  Sensor<DistanceT> mSensor;
  Config mConfig;
  NewPing& mSonar;
};
