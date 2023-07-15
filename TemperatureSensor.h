#pragma once

#include <DHT.h>
#include <stdint.h>

#include "Component.h"
#include "ConfigItem.h"
#include "Sensor.h"
#include "Util.h"

using TemperatureT = int16_t;  // Degree C

class TemperatureSensor : public IComponent {
 public:
  virtual ~TemperatureSensor() = default;

  TemperatureSensor() = delete;

  TemperatureSensor(uint8_t entityId, const char *name, DHT &dht)
      : mSensor{Sensor<TemperatureT>(
            entityId, name, SensorDeviceClass::temperature, Unit::Type::C, 1)},
        mDht{dht} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigItemValuesMsg(uint8_t *buffer) final;

  uint8_t getDiscoveryMsg(uint8_t *buffer) final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  uint8_t getValueMsg(uint8_t *buffer) final {
    return mSensor.getValueMsg(buffer);
  }

  size_t print(Stream &stream) final { return mSensor.print(stream); };

  size_t print(Stream &stream, uint8_t service) final {
    (void)stream;
    (void)service;
    return 0;
  };

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t *buffer) final;

  void setReported() final { mSensor.setReported(); }

  bool update() final;

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

  Sensor<TemperatureT> mSensor;
  Config mConfig;
  DHT &mDht;
};
