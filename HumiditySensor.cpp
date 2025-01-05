#include "HumiditySensor.h"

#include <Arduino.h>
#include <assert.h>

#include "Sensor.h"
#include "Util.h"

// TODO: Replace this ugly workaround with something better
#if (!defined HAVE_HWSERIAL0) && (!defined ARDUINO_ARCH_SAMD)
#include "extras/tests/mocks/BufferSerial.h"
extern BufferSerial Serial;
#endif

bool HumiditySensor::update() {
  if (mDht.read()) {
    int16_t newValue =
        round(mDht.readHumidity()) + mConfig.compensation.getValue();
    if (newValue < 0) {
      newValue = 0;
    }
    if (newValue > 100) {
      newValue = 100;
    }

    mSensor.setValue(static_cast<HumidityT>(newValue));
  } else {
    printMillis(Serial);
    Serial.print(F("WARN: DHT checksum fail"));
  }

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

uint8_t HumiditySensor::getConfigItemValues(ConfigItemValueT *items,
                                            uint8_t length) const {
  assert(mConfig.numberOfConfigItems <= length);

  mConfig.reportHysteresis.getConfigItemValue(&items[0]);
  mConfig.measureInterval.getConfigItemValue(&items[1]);
  mConfig.reportInterval.getConfigItemValue(&items[2]);
  mConfig.compensation.getConfigItemValue(&items[3]);

  return mConfig.numberOfConfigItems;
}

void HumiditySensor::getDiscoveryItem(DiscoveryItemT *item) const {
  assert(mConfig.numberOfConfigItems <=
         sizeof(item->configItems) / sizeof(item->configItems[0]));

  mSensor.getDiscoveryEntityItem(&item->entity);
  item->numberOfConfigItems = mConfig.numberOfConfigItems;
  mConfig.reportHysteresis.getDiscoveryConfigItem(&item->configItems[0]);
  mConfig.measureInterval.getDiscoveryConfigItem(&item->configItems[1]);
  mConfig.reportInterval.getDiscoveryConfigItem(&item->configItems[2]);
  mConfig.compensation.getDiscoveryConfigItem(&item->configItems[3]);
}

bool HumiditySensor::setConfigItemValues(const ConfigItemValueT *items,
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
      case 3:
        (void)mConfig.compensation.setConfigItemValue(&items[i]);
        break;
      default:
        return false;
    }
  }

  return true;
}

void HumiditySensor::loadConfigValues() {
  mConfig.reportHysteresis.load();
  mConfig.measureInterval.load();
  mConfig.reportInterval.load();
  mConfig.compensation.load();
}
