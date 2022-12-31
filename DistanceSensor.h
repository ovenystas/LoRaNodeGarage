/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <NewPing.h>
#include <stdint.h>

#include "ConfigItem.h"
#include "Sensor.h"
#include "Util.h"

using DistanceT = int16_t;  // cm

class DistanceSensor : public Sensor<DistanceT> {
 public:
  DistanceSensor(uint8_t entityId, const char* name, NewPing& sonar)
      : Sensor<DistanceT>(entityId, name, Unit::Type::cm), mSonar{sonar} {}

  bool update() final;

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  virtual uint8_t getConfigItemValuesMsg(uint8_t* buffer) final;

  virtual void setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {3};

    ConfigItem<DistanceT> reportHysteresis = {
        ConfigItem<DistanceT>(0, 10, Unit::Type::cm, 0)};

    ConfigItem<uint16_t> measureInterval = {
        ConfigItem<uint16_t>(1, 60, Unit::Type::s, 0)};

    ConfigItem<uint16_t> reportInterval = {
        ConfigItem<uint16_t>(2, 60, Unit::Type::s, 0)};
  };

  Config mConfig;
  NewPing& mSonar;
};
