/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "DistanceSensor.h"
#include "Util.h"
#include "components/Sensor.h"

bool DistanceSensor::update() {
  DistanceT newValue = static_cast<DistanceT>(mSonar.ping_cm());

  bool largeChange = absDiffSinceReportedValue(newValue)
      > mConfig.reportHysteresis.getValue();

  setValue(newValue);

  bool reportIsDue = timeSinceLastReport() > mConfig.reportInterval.getValue();

  return (largeChange || reportIsDue);
}

uint8_t DistanceSensor::getDiscoveryMsg(uint8_t* buffer) {
  uint8_t length = Sensor::getDiscoveryMsg(buffer);
  uint8_t& numberOfConfigs = buffer[length++];

  length += mConfig.reportHysteresis.writeDiscoveryItem(&buffer[length]);
  length += mConfig.measureInterval.writeDiscoveryItem(&buffer[length]);
  length += mConfig.reportInterval.writeDiscoveryItem(&buffer[length]);

  numberOfConfigs = 3;

  return length;
}
