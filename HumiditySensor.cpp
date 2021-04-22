/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "HumiditySensor.h"
#include "Util.h"
#include "components/Sensor.h"

bool HumiditySensor::update() {
  HumidityT newValue = round(mDht.readHumidity()) + mConfig.compensation.getValue();

  bool largeChange = absDiffSinceReportedValue(newValue)
      > mConfig.reportHysteresis.getValue();

  setValue(newValue);

  bool reportIsDue = timeSinceLastReport() > mConfig.reportInterval.getValue();

  return (largeChange || reportIsDue);
}

uint8_t HumiditySensor::getDiscoveryMsg(uint8_t* buffer) {
  uint8_t length = Sensor::getDiscoveryMsg(buffer);
  uint8_t& numberOfConfigs = buffer[length++];

  length += mConfig.reportHysteresis.writeDiscoveryItem(&buffer[length]);
  length += mConfig.measureInterval.writeDiscoveryItem(&buffer[length]);
  length += mConfig.reportInterval.writeDiscoveryItem(&buffer[length]);
  length += mConfig.compensation.writeDiscoveryItem(&buffer[length]);

  numberOfConfigs = 4;

  return length;
}
