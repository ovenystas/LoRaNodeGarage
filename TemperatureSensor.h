#pragma once

#include <DHT.h>
#include <stdint.h>

#include "ConfigItem.h"
#include "Sensor.h"
#include "Util.h"

using TemperatureT = int16_t;  // Degree C

class ITemperatureSensor : public virtual ISensor<TemperatureT> {
 public:
  virtual ~ITemperatureSensor() = default;

  virtual bool update() = 0;

  virtual uint8_t getConfigItemValuesMsg(uint8_t *buffer) = 0;

  virtual bool setConfigs(uint8_t numberOfConfigs, const uint8_t *buffer) = 0;
};

class TemperatureSensor : public virtual ITemperatureSensor,
                          public Sensor<TemperatureT> {
 public:
  TemperatureSensor() = delete;

  TemperatureSensor(uint8_t entityId, const char *name, DHT &dht)
      : Sensor<TemperatureT>(entityId, name, SensorDeviceClass::temperature,
                             Unit::Type::C, 1),
        mDht{dht} {}

  bool update() final;

  uint8_t getDiscoveryMsg(uint8_t *buffer) final;

  uint8_t getConfigItemValuesMsg(uint8_t *buffer) final;

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t *buffer) final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {4};

    ConfigItem<TemperatureT> reportHysteresis = {
        ConfigItem<TemperatureT>(0, 10, Unit::Type::C, 1)};

    ConfigItem<uint16_t> measureInterval = {
        ConfigItem<uint16_t>(1, 60, Unit::Type::s, 0)};

    ConfigItem<uint16_t> reportInterval = {
        ConfigItem<uint16_t>(2, 60, Unit::Type::s, 0)};

    ConfigItem<TemperatureT> compensation = {
        ConfigItem<TemperatureT>(3, 0, Unit::Type::C, 0)};
  };

  Config mConfig;
  DHT &mDht;
};
