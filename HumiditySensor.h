/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include <DHT.h>

#include "components/Sensor.h"
#include "Util.h"

using HumidityT = int8_t; // %

class HumiditySensor: public Sensor<HumidityT> {
public:
  HumiditySensor(uint8_t entityId, const char* name, DHT& dht) :
      Sensor<HumidityT>(entityId, name, Unit::Type::percent), mDht { dht } {
  }

  bool update() override;

  DeviceClass getDeviceClass() const override {
    return DeviceClass::humidity;
  }

private:
  struct Config {
    HumidityT compensation = { };       // %
    HumidityT reportHysteresis = { 2 }; // %
    uint16_t measureInterval = { 60 }; // s
    uint16_t reportInterval = { 60 };  // s
  };

  Config mConfig;
  DHT& mDht;
};
