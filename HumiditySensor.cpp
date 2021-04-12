/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#include <Arduino.h>
#include "HumiditySensor.h"
#include "Util.h"

bool HumiditySensor::update() {
  HumidityT newValue = round(mDht.readHumidity()) + mConfig.compensation;

  bool largeChange = (abs(newValue - mLastReportedValue)
      > mConfig.reportHysteresis);

  mValue = newValue;

  bool reportIsDue = (seconds() - mLastReportTime) > mConfig.reportInterval;

  return (largeChange || reportIsDue);
}

