#include "DistanceSensor.h"

#include <Arduino.h>
#include <assert.h>

#include "Ee.h"
#include "Sensor.h"
#include "Util.h"

bool DistanceSensor::update() {
  const DistanceT newValue = static_cast<DistanceT>(mSonar.ping_cm());

  mSensor.setValue(newValue);

  const bool largeChange = mReportHysteresis.getValue() > 0
                               ? mSensor.absDiffLastReportedValue() >=
                                     mReportHysteresis.getValue()
                               : false;

  const bool timeToReport = mReportInterval.getValue() > 0
                                ? (mSensor.timeSinceLastReport() / 1000) >=
                                      mReportInterval.getValue()
                                : false;

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
    Serial.print(mReportHysteresis.getValue());
    Serial.print("), time(");
    Serial.print(mSensor.timeSinceLastReport() / 1000);
    Serial.print(" < ");
    Serial.print(mReportInterval.getValue());
    Serial.println(")");
  }
  mSensor.setIsReportDue(isReportDue);

  return isReportDue;
}

uint8_t DistanceSensor::getDiscoveryItems(DiscoveryEntityItemT* items, uint8_t length) const {
  assert(sNumItems <= length);

  mSensor.getDiscoveryEntityItem(&items[0]);
  mReportHysteresis.getDiscoveryEntityItem(&items[1]);
  mMeasureInterval.getDiscoveryEntityItem(&items[2]);
  mReportInterval.getDiscoveryEntityItem(&items[3]);

  return sNumItems;
}

uint8_t DistanceSensor::getConfigValueItems(ValueItemT* items,
                                            uint8_t length) const {
  assert(sNumConfigItems <= length);

  mReportHysteresis.getValueItem(&items[0]);
  mMeasureInterval.getValueItem(&items[1]);
  mReportInterval.getValueItem(&items[2]);

  return sNumConfigItems;
}

bool DistanceSensor::setValueItem(const ValueItemT &item) {
  switch (item.entityId - mSensor.getEntityId() - 1) {
    case 0:
    mReportHysteresis.setValueItem(item);
    break;
    case 1:
    mMeasureInterval.setValueItem(item);
    break;
    case 2:
    mReportInterval.setValueItem(item);
    break;
    default:
      return false;
  }

  return true;
}

void DistanceSensor::loadConfigValues() {
  Ee::loadValue(EE_ADDRESS_CONFIG_DISTANCE_SENSOR_0, mReportHysteresis,
    DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT);

  Ee::loadValue(EE_ADDRESS_CONFIG_DISTANCE_SENSOR_1, mMeasureInterval,
    DistanceSensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT);

  Ee::loadValue(EE_ADDRESS_CONFIG_DISTANCE_SENSOR_2, mReportInterval,
    DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT);
}
