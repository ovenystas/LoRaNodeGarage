/*
 *  Created on: 7 mar. 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "TemperatureSensor.h"
#include "Util.h"

bool TemperatureSensor::update() {
  TemperatureT newValue = round(mDht.readTemperature() * 10) + mConfig.compensation;
  bool largeChange = (abs(newValue - mLastReportedValue) > mConfig.reportHysteresis);
  mValue = newValue;
  bool reportIsDue = (seconds() - mLastReportTime) > mConfig.reportInterval;
  return (largeChange || reportIsDue);
}

