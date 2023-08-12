#pragma once

#include <DHT.h>
#include <stdint.h>

#include "Component.h"
#include "ConfigItem.h"
#include "EeAdressMap.h"
#include "Sensor.h"
#include "Util.h"

using TemperatureT = int16_t;  // Degree C

namespace TemperatureSensorConstants {
static const TemperatureT CONFIG_REPORT_HYSTERESIS_DEFAULT = 50;
static const uint16_t CONFIG_MEASURE_INTERVAL_DEFAULT = 60;
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
static const TemperatureT CONFIG_COMPENSATION_DEFAULT = 0;
}  // namespace TemperatureSensorConstants

class TemperatureSensor : public IComponent {
 public:
  virtual ~TemperatureSensor() = default;

  TemperatureSensor() = delete;

  TemperatureSensor(uint8_t entityId, const char *name, DHT &dht)
      : mSensor{Sensor<TemperatureT>(
            entityId, name, SensorDeviceClass::temperature, Unit::Type::C, 1)},
        mDht{dht} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigItemValues(ConfigItemValueT *items,
                              uint8_t length) const final;

  void getDiscoveryItem(DiscoveryItemT *item) const final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  void getValueItem(ValueItemT *item) const final {
    return mSensor.getValueItem(item);
  }

  bool isReportDue() const final { return mSensor.isReportDue(); }

  size_t print(Stream &stream) const final { return mSensor.print(stream); };

  size_t print(Stream &stream, uint8_t service) const final {
    (void)stream;
    (void)service;
    return 0;
  };

  bool setConfigItemValues(const ConfigItemValueT *items, uint8_t length) final;

  void setReported() final { mSensor.setReported(); }

  bool update() final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {4};

    ConfigItem<TemperatureT> reportHysteresis = {ConfigItem<TemperatureT>(
        0, EE_ADDRESS_CONFIG_TEMPERATURESENSOR_0,
        TemperatureSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT,
        Unit::Type::C, 1)};

    ConfigItem<uint16_t> measureInterval = {ConfigItem<uint16_t>(
        1, EE_ADDRESS_CONFIG_TEMPERATURESENSOR_1,
        TemperatureSensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT,
        Unit::Type::s)};

    ConfigItem<uint16_t> reportInterval = {ConfigItem<uint16_t>(
        2, EE_ADDRESS_CONFIG_TEMPERATURESENSOR_2,
        TemperatureSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT,
        Unit::Type::s)};

    ConfigItem<TemperatureT> compensation = {ConfigItem<TemperatureT>(
        3, EE_ADDRESS_CONFIG_TEMPERATURESENSOR_3,
        TemperatureSensorConstants::CONFIG_COMPENSATION_DEFAULT, Unit::Type::C,
        1)};
  };

  Sensor<TemperatureT> mSensor;
  Config mConfig;
  DHT &mDht;
};
