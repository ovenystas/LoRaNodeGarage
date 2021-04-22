/*
 *  Created on: 7 mar. 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "TemperatureSensor.h"
#include "Util.h"
#include "components/Sensor.h"
#include "components/ConfigItem.h"

bool TemperatureSensor::update() {
  TemperatureT newValue = round(mDht.readTemperature() * 10)
      + mConfig.compensation.getValue();

  bool largeChange = absDiffSinceReportedValue(newValue)
      > mConfig.reportHysteresis.getValue();

  setValue(newValue);

  bool reportIsDue = timeSinceLastReport() > mConfig.reportInterval.getValue();

  return (largeChange || reportIsDue);
}

uint8_t TemperatureSensor::getDiscoveryMsg(uint8_t* buffer) {
  uint8_t length = Sensor::getDiscoveryMsg(buffer);
  uint8_t& numberOfConfigs = buffer[length++];

  length += mConfig.reportHysteresis.writeDiscoveryItem(&buffer[length]);
  length += mConfig.measureInterval.writeDiscoveryItem(&buffer[length]);
  length += mConfig.reportInterval.writeDiscoveryItem(&buffer[length]);
  length += mConfig.compensation.writeDiscoveryItem(&buffer[length]);

  numberOfConfigs = 4;

  return length;
}
