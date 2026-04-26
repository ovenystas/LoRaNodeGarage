#include "TemperatureSensor.h"

#include <Arduino.h>
#include <assert.h>

#include "Ee.h"
#include "Sensor.h"
#include "Util.h"

// TODO: Replace this ugly workaround with something better
#ifndef HAVE_HWSERIAL0
#include "BufferSerial.h"
extern BufferSerial Serial;
#endif

bool TemperatureSensor::update() {
  if (mAhtReader.isReadSuccessful()) {
    TemperatureT newValue = round(mAhtReader.getTemperature() * 10);
    mSensor.setValue(newValue);
  } else {
    Serial.println(
        F("TemperatureSensor: AHT read unsuccessful, not updating value"));
  }

  bool largeChange =
      mSensor.absDiffLastReportedValue() >=
      TemperatureSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT;

  bool timeToReport =
      (mSensor.timeSinceLastReport() / 1000) >=
      TemperatureSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT;

  bool isReportDue = largeChange || timeToReport;
  mSensor.setIsReportDue(isReportDue);

  return isReportDue;
}

bool TemperatureSensor::setValueItem(const ValueItemT& item) { return false; }

bool TemperatureSensor::getDiscoveryEntity(DiscoveryEntityT& item) const {
  mSensor.getDiscoveryEntity(item);
  return true;
}

void TemperatureSensor::loadConfigValues() {}
