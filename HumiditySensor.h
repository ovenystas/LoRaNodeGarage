/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include <DHT.h>

#include "components/Sensor.h"
#include "Util.h"

typedef int8_t HumidityT; // %

class HumiditySensor: public Sensor<HumidityT> {
public:
  HumiditySensor() = delete;
  HumiditySensor(uint8_t entityId, DHT &dht) :
    Sensor<HumidityT>(entityId), mDht { dht } {
  }

  bool update();

  DeviceClass getDeviceClass() const override {
    return DeviceClass::Humidity;
  }
  virtual Unit getUnit() const {
    return Unit::Percent;
  }

private:
  typedef struct {
    HumidityT compensation = { };       // %
    HumidityT reportHysteresis = { 2 }; // %
    uint16_t reportInterval = { 60 };  // s
  } ConfigT;

  ConfigT mConfig;
  DHT &mDht;
};
