/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include "HeightSensor.h"
#include <Arduino.h>
#include "Util.h"

bool HeightSensor::update() {
  auto newValue = mConfig.zeroValue - mDistanseSensor.getValue();

  bool largeChange = (abs(newValue - mLastReportedValue)
      > mConfig.reportHysteresis);

  mValue = newValue;

  bool reportIsDue = (seconds() - mLastReportTime) > mConfig.reportInterval;

  return (largeChange || reportIsDue);
}
