/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "DistanceSensor.h"
#include "Util.h"
#include "Sensor.h"

bool DistanceSensor::update() {
  DistanceT newValue = static_cast<DistanceT>(mSonar.ping_cm());

  bool largeChange = absDiffSinceReportedValue(newValue)
      > mConfig.reportHysteresis.getValue();

  setValue(newValue);

  bool reportIsDue = timeSinceLastReport() > mConfig.reportInterval.getValue();

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

void DistanceSensor::setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) {
  if (numberOfConfigs != mConfig.numberOfConfigItems) {
    return;
  }
  const uint8_t* p = buffer;
  p += mConfig.reportHysteresis.setConfigValue(p[0], &p[1]);
  p += mConfig.measureInterval.setConfigValue(p[0], &p[1]);
  p += mConfig.reportInterval.setConfigValue(p[0], &p[1]);
}
