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
  if (mAhtReader.isReadSuccessful()) {
    int16_t newValue = round(mAhtReader.getHumidity());
    if (newValue < 0) {
      newValue = 0;
    }
    if (newValue > 100) {
      newValue = 100;
    }

    mSensor.setValue(static_cast<HumidityT>(newValue));
  }

  bool largeChange = mSensor.absDiffLastReportedValue() >=
                     HumiditySensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT;

  bool timeToReport = (mSensor.timeSinceLastReport() / 1000) >=
                      HumiditySensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT;

  bool isReportDue = largeChange || timeToReport;
  mSensor.setIsReportDue(isReportDue);

  return isReportDue;
}

bool HumiditySensor::setValueItem(const ValueItemT& item) { return false; }

bool HumiditySensor::getDiscoveryEntity(DiscoveryEntityT& item) const {
  mSensor.getDiscoveryEntity(item);
  return true;
}
