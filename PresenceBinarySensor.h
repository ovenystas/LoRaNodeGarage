#pragma once

#include <stdint.h>

#include "BinarySensor.h"
#include "HeightSensor.h"

class IPresenceBinarySensor : public virtual IBinarySensor {
 public:
  virtual ~IPresenceBinarySensor() = default;
};

class PresenceBinarySensor : public virtual IPresenceBinarySensor,
                             public BinarySensor {
 public:
  PresenceBinarySensor() = delete;

  PresenceBinarySensor(uint8_t entityId, const char* name,
                       IHeightSensor& heightSensor)
      : BinarySensor(entityId, name, BinarySensorDeviceClass::presence),
        mHeightSensor{heightSensor} {}

  bool update() final;

  uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) final;

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {4};

    ConfigItem<HeightT> lowLimit = {
        ConfigItem<HeightT>(0, 180, Unit::Type::cm, 0)};

    ConfigItem<HeightT> highLimit = {
        ConfigItem<HeightT>(1, 200, Unit::Type::cm, 0)};

    ConfigItem<uint16_t> minStableTime = {
        ConfigItem<uint16_t>(2, 10000, Unit::Type::ms, 0)};

    ConfigItem<uint16_t> reportInterval = {
        ConfigItem<uint16_t>(3, 60, Unit::Type::s, 0)};
  };

  Config mConfig;
  IHeightSensor& mHeightSensor;
  uint32_t mLastChangedTime = {};
  bool mStableState = {};
};
