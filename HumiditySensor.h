#pragma once

#include <DHT.h>
#include <stdint.h>

#include "Component.h"
#include "ConfigItem.h"
#include "EeAdressMap.h"
#include "Sensor.h"
#include "Util.h"

using HumidityT = uint8_t;  // %

namespace HumiditySensorConstants {
static const HumidityT CONFIG_REPORT_HYSTERESIS_DEFAULT = 10;
static const uint16_t CONFIG_MEASURE_INTERVAL_DEFAULT = 60;
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
static const HumidityT CONFIG_COMPENSATION_DEFAULT = 0;
}  // namespace HumiditySensorConstants

class HumiditySensor : public IComponent {
 public:
  HumiditySensor() = delete;

  HumiditySensor(uint8_t entityId, const char* name, DHT& dht)
      : mSensor{Sensor<HumidityT>(entityId, name, SensorDeviceClass::humidity,
                                  Unit::Type::percent)},
        mDht{dht} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigItemValues(ConfigItemValueT* items,
                              uint8_t length) const final;

  void getDiscoveryItem(DiscoveryItemT* item) const final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  void getValueItem(ValueItemT* item) const final {
    return mSensor.getValueItem(item);
  }

  bool isReportDue() const final { return mSensor.isReportDue(); }

  size_t printTo(Print& p) const final { return mSensor.printTo(p); };

  size_t printTo(Print& p, uint8_t service) const final {
    (void)p;
    (void)service;
    return 0;
  };

  void loadConfigValues() final;

  bool setConfigItemValues(const ConfigItemValueT* items, uint8_t length) final;

  void setReported() final { mSensor.setReported(); }

  bool update() final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    static const uint8_t numberOfConfigItems = {4};

    ConfigItem<HumidityT> reportHysteresis = {ConfigItem<HumidityT>(
        0, EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_0,
        HumiditySensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT, 0, 100,
        Unit::Type::percent)};

    ConfigItem<uint16_t> measureInterval = {ConfigItem<uint16_t>(
        1, EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_1,
        HumiditySensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT, 0,
        Util::ONE_HOUR_IN_SECONDS, Unit::Type::s)};

    ConfigItem<uint16_t> reportInterval = {ConfigItem<uint16_t>(
        2, EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_2,
        HumiditySensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT, 0,
        Util::TWELVE_HOURS_IN_SECONDS, Unit::Type::s)};

    ConfigItem<int8_t> compensation = {
        ConfigItem<int8_t>(3, EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_3,
                           HumiditySensorConstants::CONFIG_COMPENSATION_DEFAULT,
                           -100, 100, Unit::Type::percent)};
  };

  Sensor<HumidityT> mSensor;
  Config mConfig;
  DHT& mDht;
};
