/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include <DHT.h>

#include "Sensor.h"
#include "Util.h"

typedef int16_t TemperatureT; // Degree C

class TemperatureSensor: public Sensor<TemperatureT> {
public:
  TemperatureSensor() = delete;
  TemperatureSensor(DHT &dht) :
      mDht { dht } {
  }

  bool update();

private:
  typedef struct {
    TemperatureT compensation;
    TemperatureT reportHysteresis = { 2 }; // d°C
    uint16_t reportInterval = { 60 }; // s
  } ConfigT;

  ConfigT mConfig;
  DHT &mDht;
};
