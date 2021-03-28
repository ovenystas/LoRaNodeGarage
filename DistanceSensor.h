/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include <NewPing.h>
#include "components/Sensor.h"
#include "Util.h"

typedef int16_t DistanceT; // cm

class DistanceSensor: public Sensor<DistanceT> {
public:
  DistanceSensor() = delete;
  DistanceSensor(uint8_t entityId, NewPing &sonar) :
    Sensor<DistanceT>(entityId), mSonar { sonar } {
  }
  bool update();

private:
  typedef struct {
    DistanceT reportHysteresis = { 10 }; // cm
    uint16_t reportInterval = { 60 }; // s
  } ConfigT;

  ConfigT mConfig;
  NewPing &mSonar;
};
