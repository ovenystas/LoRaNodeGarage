/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "DistanceSensor.h"
#include "Util.h"

bool DistanceSensor::update() {
  auto newValue = static_cast<DistanceT>(mSonar.ping_cm());
  bool largeChange = (abs(newValue - mLastReportedValue) > mConfig.reportHysteresis);
  mValue = newValue;
  bool reportIsDue = (seconds() - mLastReportTime) > mConfig.reportInterval;
  return (largeChange || reportIsDue);
}
