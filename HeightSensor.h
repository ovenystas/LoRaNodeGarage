#pragma once

#include <stdint.h>

#include "Component.h"
#include "ConfigItem.h"
#include "DistanceSensor.h"
#include "Sensor.h"
#include "Unit.h"
#include "Util.h"

using HeightT = int16_t;  // cm

class HeightSensor : public IComponent {
 public:
  virtual ~HeightSensor() = default;

  HeightSensor() = delete;

  HeightSensor(uint8_t entityId, const char* name,
               ISensor<DistanceT>& distanceSensor)
      : mSensor{Sensor<HeightT>(entityId, name, SensorDeviceClass::distance,
                                Unit::Type::cm)},
        mDistanceSensor{distanceSensor} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) final;

  uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  ISensor<HeightT>& getSensor() { return mSensor; }

  uint8_t getValueMsg(uint8_t* buffer) final {
    return mSensor.getValueMsg(buffer);
  }

  void print(Stream& stream) final { mSensor.print(stream); };

  void print(Stream& stream, uint8_t service) final {
    (void)stream;
    (void)service;
  };

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final;

  void setReported() final { mSensor.setReported(); }

  bool update() final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {4};

    ConfigItem<HeightT> reportHysteresis = {
        ConfigItem<HeightT>(0, 10, Unit::Type::cm, 0)};

    ConfigItem<uint16_t> reportInterval = {
        ConfigItem<uint16_t>(1, 60, Unit::Type::s, 0)};

    ConfigItem<uint16_t> stableTime = {
        ConfigItem<uint16_t>(2, 5000, Unit::Type::ms, 0)};

    ConfigItem<HeightT> zeroValue = {
        ConfigItem<HeightT>(3, 60, Unit::Type::cm, 0)};
  };

  Sensor<HeightT> mSensor;
  Config mConfig;
  ISensor<DistanceT>& mDistanceSensor;
};
