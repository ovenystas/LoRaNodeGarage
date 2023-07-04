#pragma once

#include <stdint.h>

#include "ConfigItem.h"
#include "DistanceSensor.h"
#include "Sensor.h"
#include "Unit.h"
#include "Util.h"

using HeightT = int16_t;  // cm

class IHeightSensor : public virtual ISensor<HeightT> {
 public:
  virtual ~IHeightSensor() = default;
};

class HeightSensor : public virtual IHeightSensor, public Sensor<HeightT> {
 public:
  HeightSensor(uint8_t entityId, const char* name,
               IDistanceSensor& distanceSensor)
      : Sensor<HeightT>(entityId, name, SensorDeviceClass::distance,
                        Unit::Type::cm),
        mDistanceSensor{distanceSensor} {}

  bool update() final;

  uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) final;

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final;

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

  Config mConfig;
  IDistanceSensor& mDistanceSensor;
};
