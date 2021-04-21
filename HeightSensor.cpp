/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "HeightSensor.h"
#include "Util.h"
#include "components/Sensor.h"

bool HeightSensor::update() {
  auto newValue = mConfig.zeroValue - mDistanseSensor.getValue();

  bool largeChange = absDiffSinceReportedValue(newValue)
      > mConfig.reportHysteresis;

  setValue(newValue);

  bool reportIsDue = timeSinceLastReport() > mConfig.reportInterval;

  return (largeChange || reportIsDue);
}
