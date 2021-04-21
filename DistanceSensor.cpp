/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "DistanceSensor.h"
#include "Util.h"
#include "components/Sensor.h"

bool DistanceSensor::update() {
  auto newValue = static_cast<DistanceT>(mSonar.ping_cm());

  bool largeChange = absDiffSinceReportedValue(newValue)
      > mConfig.reportHysteresis;

  setValue(newValue);

  bool reportIsDue = timeSinceLastReport() > mConfig.reportInterval;

  return (largeChange || reportIsDue);
}
