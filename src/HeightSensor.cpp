#include "HeightSensor.h"

#include <Arduino.h>
#include <assert.h>

#include "Component.h"
#include "Ee.h"
#include "Sensor.h"
#include "Util.h"

bool HeightSensor::update() {
  HeightT newValue = mZeroValue.getValue() - mDistanceSensor.getValue();

  mSensor.setValue(newValue);

  bool largeChange = mSensor.absDiffLastReportedValue() >=
                     HeightSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT;

  bool timeToReport = (mSensor.timeSinceLastReport() / 1000) >=
                      HeightSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT;

  bool isReportDue = largeChange || timeToReport;
  mSensor.setIsReportDue(isReportDue);

  return isReportDue;
}

bool HeightSensor::getDiscoveryEntity(DiscoveryEntityT& item) const {
  mSensor.getDiscoveryEntity(item);
  return true;
}

bool HeightSensor::getConfigValue(ValueItemT& item, uint8_t index) const {
  assert(index < sNumConfigItems);

  switch (index) {
    case 0:
      mStableTime.getValueItem(item);
      break;
    case 1:
      mZeroValue.getValueItem(item);
      break;
    default:
      return false;
  }

  return true;
}

bool HeightSensor::setValueItem(const ValueItemT& item) {
  switch (item.entityId - mSensor.getEntityId() - 1) {
    case 0:
      mStableTime.setValueItem(item);
      break;
    case 1:
      mZeroValue.setValueItem(item);
      break;
    default:
      return false;
  }

  return true;
}
