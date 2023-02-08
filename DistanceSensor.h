#pragma once

#include <NewPing.h>
#include <stdint.h>

#include "ConfigItem.h"
#include "Sensor.h"
#include "Util.h"

using DistanceT = int16_t;  // cm

class IDistanceSensor : public virtual ISensor<DistanceT> {
 public:
  virtual ~IDistanceSensor() = default;

  virtual bool update() = 0;

  virtual uint8_t getConfigItemValuesMsg(uint8_t* buffer) = 0;

  virtual bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) = 0;
};

class DistanceSensor : public virtual IDistanceSensor,
                       public Sensor<DistanceT> {
 public:
  DistanceSensor(uint8_t entityId, const char* name, NewPing& sonar)
      : Sensor<DistanceT>(entityId, name, SensorDeviceClass::distance,
                          Unit::Type::cm),
        mSonar{sonar} {}

  bool update() final;

  uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) final;

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {3};

    ConfigItem<DistanceT> reportHysteresis = {
        ConfigItem<DistanceT>(0, 10, Unit::Type::cm)};

    ConfigItem<uint16_t> measureInterval = {
        ConfigItem<uint16_t>(1, 60, Unit::Type::s)};

    ConfigItem<uint16_t> reportInterval = {
        ConfigItem<uint16_t>(2, 60, Unit::Type::s)};
  };

  Config mConfig;
  NewPing& mSonar;
};
