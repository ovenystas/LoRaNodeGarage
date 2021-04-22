/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "HeightSensor.h"
#include "Util.h"
#include "components/Sensor.h"

bool HeightSensor::update() {
  HeightT newValue = mConfig.zeroValue.getValue() - mDistanseSensor.getValue();

  bool largeChange = absDiffSinceReportedValue(newValue)
      > mConfig.reportHysteresis.getValue();

  setValue(newValue);

  bool reportIsDue = timeSinceLastReport() > mConfig.reportInterval.getValue();

  return (largeChange || reportIsDue);
}

uint8_t HeightSensor::getDiscoveryMsg(uint8_t* buffer) {
  uint8_t length = Sensor::getDiscoveryMsg(buffer);
  uint8_t& numberOfConfigs = buffer[length++];

  length += mConfig.reportHysteresis.writeDiscoveryItem(&buffer[length]);
  length += mConfig.reportInterval.writeDiscoveryItem(&buffer[length]);
  length += mConfig.stableTime.writeDiscoveryItem(&buffer[length]);
  length += mConfig.zeroValue.writeDiscoveryItem(&buffer[length]);

  numberOfConfigs = 4;

  return length;
}
