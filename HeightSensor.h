/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>

#include "ConfigItem.h"
#include "DistanceSensor.h"
#include "Sensor.h"
#include "Unit.h"
#include "Util.h"

using HeightT = int16_t;  // cm

class HeightSensor : public Sensor<HeightT> {
 public:
  HeightSensor(uint8_t entityId, const char* name,
               DistanceSensor& distanceSensor)
      : Sensor<HeightT>(entityId, name, Unit::Type::cm),
        mDistanseSensor{distanceSensor} {}

  bool update() final;

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  virtual uint8_t getConfigItemValuesMsg(uint8_t* buffer) final;

  virtual bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {4};

    ConfigItem<DistanceT> reportHysteresis = {
        ConfigItem<DistanceT>(0, 10, Unit::Type::cm, 0)};

    ConfigItem<uint16_t> reportInterval = {
        ConfigItem<uint16_t>(1, 60, Unit::Type::s, 0)};

    ConfigItem<uint16_t> stableTime = {
        ConfigItem<uint16_t>(2, 5000, Unit::Type::ms, 0)};

    ConfigItem<HeightT> zeroValue = {
        ConfigItem<HeightT>(3, 60, Unit::Type::cm, 0)};
  };

  Config mConfig;
  DistanceSensor& mDistanseSensor;
};
