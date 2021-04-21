/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "HumiditySensor.h"
#include "Util.h"
#include "components/Sensor.h"

bool HumiditySensor::update() {
  HumidityT newValue = round(mDht.readHumidity()) + mConfig.compensation;

  bool largeChange = absDiffSinceReportedValue(newValue)
      > mConfig.reportHysteresis;

  setValue(newValue);

  bool reportIsDue = timeSinceLastReport() > mConfig.reportInterval;

  return (largeChange || reportIsDue);
}

