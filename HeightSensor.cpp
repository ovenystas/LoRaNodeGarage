#include "HeightSensor.h"

#include <Arduino.h>
#include <assert.h>

#include "Sensor.h"
#include "Util.h"

bool HeightSensor::update() {
  HeightT newValue = mConfig.zeroValue.getValue() - mDistanceSensor.getValue();

  mSensor.setValue(newValue);

  bool largeChange = mConfig.reportHysteresis.getValue() > 0
                         ? mSensor.absDiffLastReportedValue() >=
                               mConfig.reportHysteresis.getValue()
                         : false;

  bool timeToReport = mConfig.reportInterval.getValue() > 0
                          ? mSensor.timeSinceLastReport() >=
                                (mConfig.reportInterval.getValue() * 1000)
                          : false;

  bool isReportDue = largeChange || timeToReport;
  mSensor.setIsReportDue(isReportDue);

  return isReportDue;
}

void HeightSensor::getDiscoveryItem(DiscoveryItemT* item) const {
  assert(mConfig.numberOfConfigItems <=
         sizeof(item->configItems) / sizeof(item->configItems[0]));

  mSensor.getDiscoveryEntityItem(&item->entity);
  item->numberOfConfigItems = mConfig.numberOfConfigItems;
  mConfig.reportHysteresis.getDiscoveryConfigItem(&item->configItems[0]);
  mConfig.reportInterval.getDiscoveryConfigItem(&item->configItems[1]);
  mConfig.stableTime.getDiscoveryConfigItem(&item->configItems[2]);
  mConfig.zeroValue.getDiscoveryConfigItem(&item->configItems[3]);
}

uint8_t HeightSensor::getConfigItemValues(ConfigItemValueT* items,
                                          uint8_t length) const {
  assert(mConfig.numberOfConfigItems <= length);

  mConfig.reportHysteresis.getConfigItemValue(&items[0]);
  mConfig.reportInterval.getConfigItemValue(&items[1]);
  mConfig.stableTime.getConfigItemValue(&items[2]);
  mConfig.zeroValue.getConfigItemValue(&items[3]);

  return mConfig.numberOfConfigItems;
}

bool HeightSensor::setConfigItemValues(const ConfigItemValueT* items,
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
        (void)mConfig.reportInterval.setConfigItemValue(&items[i]);
        break;
      case 2:
        (void)mConfig.stableTime.setConfigItemValue(&items[i]);
        break;
      case 3:
        (void)mConfig.zeroValue.setConfigItemValue(&items[i]);
        break;
      default:
        return false;
    }
  }

  return true;
}

void HeightSensor::loadConfigValues() {
  mConfig.reportHysteresis.load();
  mConfig.reportInterval.load();
  mConfig.stableTime.load();
  mConfig.zeroValue.load();
}
