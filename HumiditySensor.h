/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <DHT.h>
#include <stdint.h>

#include "ConfigItem.h"
#include "Sensor.h"
#include "Util.h"

using HumidityT = int8_t;  // %

class HumiditySensor : public Sensor<HumidityT> {
 public:
  HumiditySensor(uint8_t entityId, const char* name, DHT& dht)
      : Sensor<HumidityT>(entityId, name, Unit::Type::percent), mDht{dht} {}

  bool update() final;

  inline DeviceClass getDeviceClass() const final {
    return DeviceClass::humidity;
  }

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  virtual uint8_t getConfigItemValuesMsg(uint8_t* buffer) final;

  virtual void setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final;

 private:
  struct Config {
    const uint8_t numberOfConfigItems = {4};

    ConfigItem<HumidityT> reportHysteresis = {
        ConfigItem<HumidityT>(0, 2, Unit::Type::percent, 0)};

    ConfigItem<uint16_t> measureInterval = {
        ConfigItem<uint16_t>(1, 60, Unit::Type::s, 0)};

    ConfigItem<uint16_t> reportInterval = {
        ConfigItem<uint16_t>(2, 60, Unit::Type::s, 0)};

    ConfigItem<HumidityT> compensation = {
        ConfigItem<HumidityT>(3, 0, Unit::Type::percent, 0)};
  };

  Config mConfig;
  DHT& mDht;
};
