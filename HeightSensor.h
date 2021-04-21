/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include "components/Sensor.h"
#include "DistanceSensor.h"
#include "Util.h"

using HeightT = int16_t; // cm

class HeightSensor: public Sensor<HeightT> {
public:
  HeightSensor(uint8_t entityId, const char* name, DistanceSensor& distanceSensor) :
      Sensor<HeightT>(entityId, name, Unit::Type::cm), mDistanseSensor {
          distanceSensor } {
  }

  bool update() override;

private:
  struct Config {
    HeightT zeroValue = { 250 };        // cm
    HeightT reportHysteresis = { 10 };  // cm
    uint16_t reportInterval = { 60 };   // s
    uint16_t stableTime = { 5000 };     // ms
  };

  Config mConfig;
  DistanceSensor& mDistanseSensor;
};
