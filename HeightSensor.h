/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include "Sensor.h"
#include "DistanceSensor.h"
#include "Util.h"

typedef int16_t HeightT; // cm

class HeightSensor: public Sensor<HeightT> {
public:
  HeightSensor() = delete;
  HeightSensor(DistanceSensor &distanceSensor) :
      mDistanseSensor { distanceSensor } {
  }
  bool update();

private:
  typedef struct {
    HeightT zeroValue = { 250 };        // cm
    HeightT reportHysteresis = { 10 };  // cm
    uint16_t reportInterval = { 60 };   // s
    uint16_t stableTime = { 5000 };     // ms
  } ConfigT;

  ConfigT mConfig;
  DistanceSensor &mDistanseSensor;
};
