/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */


#pragma once

#include <stdint.h>
#include <NewPing.h>
#include "Sensor.h"
#include "Util.h"

typedef int16_t DistanceT; // cm

class DistanceSensor :public Sensor<DistanceT> {
public:
  DistanceSensor() = delete;
  DistanceSensor(NewPing& sonar) :
      mSonar { sonar } {
  }
  bool update();

private:
  typedef struct {
    DistanceT reportHysteresis = { 10 }; // cm
    uint16_t reportInterval = { 60 }; // s
  } ConfigT;

  ConfigT mConfig;
  NewPing& mSonar;
};
