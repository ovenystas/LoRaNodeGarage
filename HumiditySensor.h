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

  HumiditySensor(uint8_t entityId, const char* name, DHT& dht) :
      Sensor<HumidityT>(entityId, name, Unit::TypeE::Percent), mDht { dht } {
  }

  bool update() override;

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
    uint16_t measureInterval = { 60 }; // s
    uint16_t reportInterval = { 60 };  // s
  } ConfigT;

  ConfigT mConfig;
  DHT& mDht;
};
