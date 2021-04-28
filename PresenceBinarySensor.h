/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>
#include "components/BinarySensor.h"
#include "HeightSensor.h"

class PresenceBinarySensor: public BinarySensor {
public:
  PresenceBinarySensor() = delete;

  PresenceBinarySensor(uint8_t entityId, const char* name, HeightSensor& heightSensor) :
      BinarySensor(entityId, name), mHeightSensor { heightSensor } {
  }

  bool update() final;

  inline DeviceClass getDeviceClass() const final {
    return DeviceClass::presence;
  }

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) final;

private:
  struct Config {
    ConfigItem<HeightT> lowLimit =
      { ConfigItem<HeightT>(0, 180, Unit::Type::cm, 0) };

    ConfigItem<HeightT> highLimit =
      { ConfigItem<HeightT>(1, 200, Unit::Type::cm, 0) };

    ConfigItem<uint16_t> minStableTime =
      { ConfigItem<uint16_t>(2, 10000, Unit::Type::ms, 0) };
  };

  Config mConfig;
  HeightSensor& mHeightSensor;
  uint32_t mLastChangedTime = { };
  bool mStableState = { };
};
