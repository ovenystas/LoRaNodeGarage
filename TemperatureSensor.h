/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include <DHT.h>

#include "components/Sensor.h"
#include "Util.h"

using TemperatureT = int16_t; // Degree C

class TemperatureSensor: public Sensor<TemperatureT> {
public:
  TemperatureSensor() = delete;

  TemperatureSensor(uint8_t entityId, const char* name, DHT& dht) :
      Sensor<TemperatureT>(entityId, name, Unit::Type::C, 1), mDht { dht } {
  }

  bool update() override;

  DeviceClass getDeviceClass() const override {
    return DeviceClass::temperature;
  }

private:
  struct Config {
    TemperatureT compensation;
    TemperatureT reportHysteresis = { 2 }; // d°C
    uint16_t measureInterval = { 60 }; // s
    uint16_t reportInterval = { 60 }; // s
  };

  Config mConfig;
  DHT& mDht;
};
