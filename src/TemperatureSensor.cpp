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
  if (mDhtReader.isReadSuccessful()) {
    TemperatureT newValue =
        round(mDhtReader.getTemperature() * 10) + mCompensation.getValue();

    mSensor.setValue(newValue);
  }

  bool largeChange = mReportHysteresis.getValue() > 0
                         ? mSensor.absDiffLastReportedValue() >=
                               mReportHysteresis.getValue()
                         : false;

  bool timeToReport = mReportInterval.getValue() > 0
                          ? (mSensor.timeSinceLastReport() / 1000)>=
                                mReportInterval.getValue()
                          : false;

  bool isReportDue = largeChange || timeToReport;
  mSensor.setIsReportDue(isReportDue);

  return isReportDue;
}

uint8_t TemperatureSensor::getConfigValueItems(ValueItemT* items,
                                            uint8_t length) const {
  assert(sNumConfigItems <= length);

  mReportHysteresis.getValueItem(&items[0]);
  mMeasureInterval.getValueItem(&items[1]);
  mReportInterval.getValueItem(&items[2]);
  mCompensation.getValueItem(&items[3]);

  return sNumConfigItems;
}

bool TemperatureSensor::setValueItem(const ValueItemT &item) {
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
      case 3:
        mCompensation.setValueItem(item);
        break;
      default:
        return false;
    }

  return true;
}

uint8_t TemperatureSensor::getDiscoveryItems(DiscoveryEntityItemT* items, uint8_t length) const {
  assert(sNumItems <= length);

  mSensor.getDiscoveryEntityItem(&items[0]);
  mReportHysteresis.getDiscoveryEntityItem(&items[1]);
  mMeasureInterval.getDiscoveryEntityItem(&items[2]);
  mReportInterval.getDiscoveryEntityItem(&items[3]);
  mCompensation.getDiscoveryEntityItem(&items[4]);

  return sNumItems;
}

void TemperatureSensor::loadConfigValues() {
  Ee::loadValue(EE_ADDRESS_CONFIG_TEMPERATURE_SENSOR_0, mReportHysteresis,
    TemperatureSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT);

  Ee::loadValue(EE_ADDRESS_CONFIG_TEMPERATURE_SENSOR_1, mMeasureInterval,
    TemperatureSensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT);

  Ee::loadValue(EE_ADDRESS_CONFIG_TEMPERATURE_SENSOR_2, mReportInterval,
    TemperatureSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT);

  Ee::loadValue(EE_ADDRESS_CONFIG_TEMPERATURE_SENSOR_3, mCompensation,
    TemperatureSensorConstants::CONFIG_COMPENSATION_DEFAULT);
}
