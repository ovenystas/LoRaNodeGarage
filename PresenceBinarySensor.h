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

  bool update() override;

  virtual DeviceClass getDeviceClass() const {
    return DeviceClass::Presence;
  }

private:
  typedef struct {
    HeightT lowLimit = { 180 };
    HeightT highLimit = { 200 };
    uint16_t minTime = { 10000 };
  } ConfigT;

  ConfigT mConfig;
  HeightSensor& mHeightSensor;
  uint32_t mLastChangedTime = { };
  bool mStableState = { };
};
