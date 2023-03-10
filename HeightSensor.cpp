/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include "HeightSensor.h"

#include <Arduino.h>

#include "Sensor.h"
#include "Util.h"

bool HeightSensor::update() {
  HeightT newValue = mConfig.zeroValue.getValue() - mDistanseSensor.getValue();

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

uint8_t HeightSensor::getDiscoveryMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  p += Sensor::getDiscoveryMsg(p);
  *p++ = mConfig.numberOfConfigItems;

  p += mConfig.reportHysteresis.writeDiscoveryItem(p);
  p += mConfig.reportInterval.writeDiscoveryItem(p);
  p += mConfig.stableTime.writeDiscoveryItem(p);
  p += mConfig.zeroValue.writeDiscoveryItem(p);

  return p - buffer;
}

uint8_t HeightSensor::getConfigItemValuesMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  *p++ = getEntityId();
  *p++ = mConfig.numberOfConfigItems;

  p += mConfig.reportHysteresis.writeConfigItemValue(p);
  p += mConfig.reportInterval.writeConfigItemValue(p);
  p += mConfig.stableTime.writeConfigItemValue(p);
  p += mConfig.zeroValue.writeConfigItemValue(p);

  return p - buffer;
}

bool HeightSensor::setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) {
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
        p += mConfig.reportInterval.setConfigValue(p[0], &p[1]);
        break;
      case 2:
        p += mConfig.stableTime.setConfigValue(p[0], &p[1]);
        break;
      case 3:
        p += mConfig.zeroValue.setConfigValue(p[0], &p[1]);
        break;
      default:
        return false;
    }
  }
  return true;
}
