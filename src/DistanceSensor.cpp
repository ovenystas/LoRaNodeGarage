#include "DistanceSensor.h"

#include <Arduino.h>
#include <assert.h>

#include "Ee.h"
#include "Sensor.h"
#include "Util.h"

bool DistanceSensor::update() {
  const DistanceT newValue = static_cast<DistanceT>(mSonar.ping_cm());

  mSensor.setValue(newValue);

  const bool largeChange =
      mSensor.absDiffLastReportedValue() >=
      DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT;

  const bool timeToReport =
      (mSensor.timeSinceLastReport() / 1000) >=
      DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT;

  const bool isReportDue = largeChange || timeToReport;

  mSensor.setIsReportDue(isReportDue);

  return isReportDue;
}

bool DistanceSensor::getDiscoveryEntity(DiscoveryEntityT& item) const {
  mSensor.getDiscoveryEntity(item);
  return true;
}

bool DistanceSensor::getConfigValue(ValueItemT& item, uint8_t index) const {
  if (index >= sNumConfigItems) {
    return false;
  }

  // DistanceSensor has no config items

  return true;
}

bool DistanceSensor::setValueItem(const ValueItemT& item) { return false; }
