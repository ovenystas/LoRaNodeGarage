/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include <NewPing.h>
#include "components/Sensor.h"
#include "Util.h"

using DistanceT = int16_t; // cm

class DistanceSensor: public Sensor<DistanceT> {
public:
  DistanceSensor(uint8_t entityId, const char* name, NewPing& sonar) :
      Sensor<DistanceT>(entityId, name, Unit::Type::cm), mSonar { sonar } {
  }

  bool update() override;

private:
  struct Config {
    DistanceT reportHysteresis = { 10 }; // cm
    uint16_t measureInterval = { 60 }; // s
    uint16_t reportInterval = { 60 }; // s
  };

  Config mConfig;
  NewPing& mSonar;
};
