/*
 *  Created on: 7 mar. 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "TemperatureSensor.h"
#include "Util.h"
#include "components/Sensor.h"

bool TemperatureSensor::update() {
  TemperatureT newValue = round(mDht.readTemperature() * 10)
      + mConfig.compensation;

  bool largeChange = absDiffSinceReportedValue(newValue)
      > mConfig.reportHysteresis;

  setValue(newValue);

  bool reportIsDue = timeSinceLastReport() > mConfig.reportInterval;

  return (largeChange || reportIsDue);
}

