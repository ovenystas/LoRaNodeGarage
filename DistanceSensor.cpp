/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include "DistanceSensor.h"

#include <Arduino.h>

#include "Sensor.h"
#include "Util.h"

bool DistanceSensor::update() {
  DistanceT newValue = static_cast<DistanceT>(mSonar.ping_cm());

  setValue(newValue);

  bool largeChange =
      mConfig.reportHysteresis.getValue() > 0
          ? absDiffLastReportedValue() >= mConfig.reportHysteresis.getValue()
          : false;

  bool reportIsDue =
      mConfig.reportInterval.getValue() > 0
          ? timeSinceLastReport() >= mConfig.reportInterval.getValue()
          : false;

  return (largeChange || reportIsDue);
}

uint8_t DistanceSensor::getDiscoveryMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  p += Sensor::getDiscoveryMsg(p);

  *p++ = mConfig.numberOfConfigItems;

  p += mConfig.reportHysteresis.writeDiscoveryItem(p);
  p += mConfig.measureInterval.writeDiscoveryItem(p);
  p += mConfig.reportInterval.writeDiscoveryItem(p);

  return p - buffer;
}

uint8_t DistanceSensor::getConfigItemValuesMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  *p++ = getEntityId();
  *p++ = mConfig.numberOfConfigItems;

  p += mConfig.reportHysteresis.writeConfigItemValue(p);
  p += mConfig.measureInterval.writeConfigItemValue(p);
  p += mConfig.reportInterval.writeConfigItemValue(p);

  return p - buffer;
}

bool DistanceSensor::setConfigs(uint8_t numberOfConfigs,
                                const uint8_t* buffer) {
  if (numberOfConfigs > mConfig.numberOfConfigItems) {
    return false;
  }
  const uint8_t* p = buffer;
  while (numberOfConfigs-- > 0) {
    switch (*p) {
      case 0:
        p += mConfig.reportHysteresis.setConfigValue(p[0], &p[1]);
        break;
      case 1:
        p += mConfig.measureInterval.setConfigValue(p[0], &p[1]);
        break;
      case 2:
        p += mConfig.reportInterval.setConfigValue(p[0], &p[1]);
        break;
      default:
        return false;
    }
  }
  return true;
}
