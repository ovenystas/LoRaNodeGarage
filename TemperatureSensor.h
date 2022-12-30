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

using TemperatureT = int16_t;  // Degree C

class TemperatureSensor : public Sensor<TemperatureT> {
 public:
  TemperatureSensor() = delete;

  TemperatureSensor(uint8_t entityId, const char* name, DHT& dht)
      : Sensor<TemperatureT>(entityId, name, Unit::Type::C, 1), mDht{dht} {}

  bool update() final;

  inline DeviceClass getDeviceClass() const final {
    return DeviceClass::temperature;
  }

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  virtual uint8_t getConfigItemValuesMsg(uint8_t* buffer) final;

  virtual void setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final;

 private:
  struct Config {
    const uint8_t numberOfConfigItems = {4};

    ConfigItem<TemperatureT> reportHysteresis = {
        ConfigItem<TemperatureT>(0, 10, Unit::Type::C, 1)};

    ConfigItem<uint16_t> measureInterval = {
        ConfigItem<uint16_t>(1, 60, Unit::Type::s, 0)};

    ConfigItem<uint16_t> reportInterval = {
        ConfigItem<uint16_t>(2, 60, Unit::Type::s, 0)};

    ConfigItem<TemperatureT> compensation = {
        ConfigItem<TemperatureT>(3, 0, Unit::Type::C, 0)};
  };

  Config mConfig;
  DHT& mDht;
};
