/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include <DHT.h>

#include "Sensor.h"
#include "Util.h"

typedef int8_t HumidityT; // %

class HumiditySensor: public Sensor<HumidityT> {
public:
  HumiditySensor() = delete;
  HumiditySensor(DHT &dht) :
      mDht { dht } {
  }

  bool update();

private:
  typedef struct {
    HumidityT compensation = { };       // %
    HumidityT reportHysteresis = { 2 }; // %
    uint16_t reportInterval = { 60 };  // s
  } ConfigT;

  ConfigT mConfig;
  DHT &mDht;
};
