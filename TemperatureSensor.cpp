/*
 *  Created on: 7 mar. 2021
 *      Author: oveny
 */

#include <Arduino.h>

#include "TemperatureSensor.h"
#include "Util.h"
#include "components/Sensor.h"
#include "components/ConfigItem.h"

bool TemperatureSensor::update() {
  TemperatureT newValue = round(mDht.readTemperature() * 10)
      + mConfig.compensation.getValue();

  bool largeChange = absDiffSinceReportedValue(newValue)
      > mConfig.reportHysteresis.getValue();

  setValue(newValue);

  bool reportIsDue = timeSinceLastReport() > mConfig.reportInterval.getValue();

  return (largeChange || reportIsDue);
}

uint8_t TemperatureSensor::getDiscoveryMsg(uint8_t* buffer) {
  uint8_t* p = buffer;
  p += Sensor::getDiscoveryMsg(p);
  *p++ = mConfig.numberOfConfigItems;

  p += mConfig.reportHysteresis.writeDiscoveryItem(p);
  p += mConfig.measureInterval.writeDiscoveryItem(p);
  p += mConfig.reportInterval.writeDiscoveryItem(p);
  p += mConfig.compensation.writeDiscoveryItem(p);

  return p - buffer;
}

uint8_t TemperatureSensor::getConfigItemValuesMsg(uint8_t* buffer) {
  uint8_t* p = buffer;

  *p++ = getEntityId();
  *p++ = mConfig.numberOfConfigItems;

  p += mConfig.reportHysteresis.writeConfigItemValue(p);
  p += mConfig.measureInterval.writeConfigItemValue(p);
  p += mConfig.reportInterval.writeConfigItemValue(p);
  p += mConfig.compensation.writeConfigItemValue(p);

  return p - buffer;
}
