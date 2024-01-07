/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include "DistanceSensor.h"

#include <Arduino.h>
#include <assert.h>

#include "Sensor.h"
#include "Util.h"

bool DistanceSensor::update() {
  const DistanceT newValue = static_cast<DistanceT>(mSonar.ping_cm());

  mSensor.setValue(newValue);

  const bool largeChange = mConfig.reportHysteresis.getValue() > 0
                               ? mSensor.absDiffLastReportedValue() >=
                                     mConfig.reportHysteresis.getValue()
                               : false;

  const bool timeToReport =
      mConfig.reportInterval.getValue() > 0
          ? mSensor.timeSinceLastReport() >= mConfig.reportInterval.getValue()
          : false;

  const bool isReportDue = largeChange || timeToReport;
  mSensor.setIsReportDue(isReportDue);

  return isReportDue;
}

void DistanceSensor::getDiscoveryItem(DiscoveryItemT* item) const {
  assert(mConfig.numberOfConfigItems <=
         sizeof(item->configItems) / sizeof(item->configItems[0]));

  mSensor.getDiscoveryEntityItem(&item->entity);
  item->numberOfConfigItems = mConfig.numberOfConfigItems;
  mConfig.reportHysteresis.getDiscoveryConfigItem(&item->configItems[0]);
  mConfig.measureInterval.getDiscoveryConfigItem(&item->configItems[1]);
  mConfig.reportInterval.getDiscoveryConfigItem(&item->configItems[2]);
}

uint8_t DistanceSensor::getConfigItemValues(ConfigItemValueT* items,
                                            uint8_t length) const {
  assert(mConfig.numberOfConfigItems <= length);

  mConfig.reportHysteresis.getConfigItemValue(&items[0]);
  mConfig.measureInterval.getConfigItemValue(&items[1]);
  mConfig.reportInterval.getConfigItemValue(&items[2]);

  return mConfig.numberOfConfigItems;
}

bool DistanceSensor::setConfigItemValues(const ConfigItemValueT* items,
                                         uint8_t length) {
  if (length > mConfig.numberOfConfigItems) {
    return false;
  }

  for (uint8_t i = 0; i < length; i++) {
    switch (items[i].configId) {
      case 0:
        (void)mConfig.reportHysteresis.setConfigItemValue(&items[i]);
        break;
      case 1:
        (void)mConfig.measureInterval.setConfigItemValue(&items[i]);
        break;
      case 2:
        (void)mConfig.reportInterval.setConfigItemValue(&items[i]);
        break;
      default:
        return false;
    }
  }

  return true;
}

void DistanceSensor::loadConfigValues() {
  mConfig.reportHysteresis.load();
  mConfig.measureInterval.load();
  mConfig.reportInterval.load();
}
