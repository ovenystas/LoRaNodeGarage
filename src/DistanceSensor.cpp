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
  if (largeChange) {
    Serial.print(F("Dist: Large change, "));
    Serial.print(mSensor.absDiffLastReportedValue());
    Serial.println(" cm");
  } else if (timeToReport) {
    Serial.print(F("Dist: Report interval elapsed, "));
    Serial.print(mSensor.timeSinceLastReport() / 1000);
    Serial.println(" s");
  } else {
    Serial.print(F("Dist: No report, change("));
    Serial.print(mSensor.absDiffLastReportedValue());
    Serial.print(" < ");
    Serial.print(DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT);
    Serial.print("), time(");
    Serial.print(mSensor.timeSinceLastReport() / 1000);
    Serial.print(" < ");
    Serial.print(DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT);
    Serial.println(")");
  }
  mSensor.setIsReportDue(isReportDue);

  return isReportDue;
}

uint8_t DistanceSensor::getDiscoveryItems(DiscoveryEntityItemT* items,
                                          uint8_t length) const {
  assert(sNumItems <= length);

  mSensor.getDiscoveryEntityItem(&items[0]);

  return sNumItems;
}

uint8_t DistanceSensor::getConfigValueItems(ValueItemT* items,
                                            uint8_t length) const {
  assert(sNumConfigItems <= length);

  return sNumConfigItems;
}

bool DistanceSensor::setValueItem(const ValueItemT& item) { return false; }

void DistanceSensor::loadConfigValues() {}
