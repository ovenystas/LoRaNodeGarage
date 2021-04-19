/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include <DHT.h>

#include "components/Sensor.h"
#include "Util.h"

typedef int16_t TemperatureT; // Degree C

class TemperatureSensor: public Sensor<TemperatureT> {
public:
  TemperatureSensor() = delete;

  TemperatureSensor(uint8_t entityId, const char* name, DHT& dht) :
      Sensor<TemperatureT>(entityId, name, Unit::TypeE::DegreeC, 1), mDht { dht } {
  }

  bool update() override;

  DeviceClass getDeviceClass() const override {
    return DeviceClass::Temperature;
  }

  virtual Unit getUnit() const {
    return Unit::DegreeC;
  }

private:
  typedef struct {
    TemperatureT compensation;
    TemperatureT reportHysteresis = { 2 }; // d°C
    uint16_t measureInterval = { 60 }; // s
    uint16_t reportInterval = { 60 }; // s
  } ConfigT;

  ConfigT mConfig;
  DHT& mDht;
};
