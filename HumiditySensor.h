#pragma once

#include <DHT.h>
#include <stdint.h>

#include "Component.h"
#include "ConfigItem.h"
#include "Sensor.h"
#include "Util.h"

using HumidityT = int8_t;  // %

class HumiditySensor : public IComponent {
 public:
  virtual ~HumiditySensor() = default;

  HumiditySensor() = delete;

  HumiditySensor(uint8_t entityId, const char* name, DHT& dht)
      : mSensor{Sensor<HumidityT>(entityId, name, SensorDeviceClass::humidity,
                                  Unit::Type::percent)},
        mDht{dht} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) final;

  uint8_t getValueMsg(uint8_t* buffer) final {
    return mSensor.getValueMsg(buffer);
  }

  size_t print(Stream& stream) final { return mSensor.print(stream); };

  size_t print(Stream& stream, uint8_t service) final {
    (void)stream;
    (void)service;
    return 0;
  };

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final;

  void setReported() final { mSensor.setReported(); }

  bool update() final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {4};

    ConfigItem<HumidityT> reportHysteresis = {
        ConfigItem<HumidityT>(0, 2, Unit::Type::percent, 0)};

    ConfigItem<uint16_t> measureInterval = {
        ConfigItem<uint16_t>(1, 60, Unit::Type::s, 0)};

    ConfigItem<uint16_t> reportInterval = {
        ConfigItem<uint16_t>(2, 60, Unit::Type::s, 0)};

    ConfigItem<HumidityT> compensation = {
        ConfigItem<HumidityT>(3, 0, Unit::Type::percent, 0)};
  };

  Sensor<HumidityT> mSensor;
  Config mConfig;
  DHT& mDht;
};
