#include "HumiditySensor.h"

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

bool HumiditySensor::update() {
  if (mDhtReader.isReadSuccessful()) {
    int16_t newValue =
        round(mDhtReader.getHumidity()) + mCompensation.getValue();
    if (newValue < 0) {
      newValue = 0;
    }
    if (newValue > 100) {
      newValue = 100;
    }

    mSensor.setValue(static_cast<HumidityT>(newValue));
  }

  bool largeChange = mReportHysteresis.getValue() > 0
                         ? mSensor.absDiffLastReportedValue() >=
                               mReportHysteresis.getValue()
                         : false;

  bool timeToReport = mReportInterval.getValue() > 0
                          ? (mSensor.timeSinceLastReport() / 1000) >=
                                mReportInterval.getValue()
                          : false;

  bool isReportDue = largeChange || timeToReport;
  mSensor.setIsReportDue(isReportDue);

  return isReportDue;
}

uint8_t HumiditySensor::getConfigValueItems(ValueItemT* items,
                                            uint8_t length) const {
  assert(sNumConfigItems <= length);

  mReportHysteresis.getValueItem(&items[0]);
  mMeasureInterval.getValueItem(&items[1]);
  mReportInterval.getValueItem(&items[2]);
  mCompensation.getValueItem(&items[3]);

  return sNumConfigItems;
}

bool HumiditySensor::setValueItem(const ValueItemT &item) {
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

uint8_t HumiditySensor::getDiscoveryItems(DiscoveryEntityItemT* items, uint8_t length) const {
  assert(sNumItems <= length);

  mSensor.getDiscoveryEntityItem(&items[0]);
  mReportHysteresis.getDiscoveryEntityItem(&items[1]);
  mMeasureInterval.getDiscoveryEntityItem(&items[2]);
  mReportInterval.getDiscoveryEntityItem(&items[3]);
  mCompensation.getDiscoveryEntityItem(&items[4]);

  return sNumItems;
}

void HumiditySensor::loadConfigValues() {
  Ee::loadValue(EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_0, mReportHysteresis,
    HumiditySensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT);

  Ee::loadValue(EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_1, mMeasureInterval,
    HumiditySensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT);

  Ee::loadValue(EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_2, mReportInterval,
    HumiditySensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT);

  Ee::loadValue(EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_3, mCompensation,
    HumiditySensorConstants::CONFIG_COMPENSATION_DEFAULT);
}
