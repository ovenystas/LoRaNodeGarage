#include "HumiditySensor.h"

#include <Arduino.h>

#include "Sensor.h"
#include "Util.h"

bool HumiditySensor::update() {
  HumidityT newValue =
      round(mDht.readHumidity()) + mConfig.compensation.getValue();

  setValue(newValue);

  bool largeChange =
      mConfig.reportHysteresis.getValue() > 0
          ? absDiffLastReportedValue() >= mConfig.reportHysteresis.getValue()
          : false;

  bool reportIsDue =
      mConfig.reportInterval.getValue() > 0
          ? timeSinceLastReport() >= mConfig.reportInterval.getValue()
          : false;

  return (largeChange || reportIsDue);
}

uint8_t HumiditySensor::getDiscoveryMsg(uint8_t *buffer) {
  uint8_t *p = buffer;
  p += Sensor::getDiscoveryMsg(p);
  *p++ = mConfig.numberOfConfigItems;

  p += mConfig.reportHysteresis.writeDiscoveryItem(p);
  p += mConfig.measureInterval.writeDiscoveryItem(p);
  p += mConfig.reportInterval.writeDiscoveryItem(p);
  p += mConfig.compensation.writeDiscoveryItem(p);

  return p - buffer;
}

uint8_t HumiditySensor::getConfigItemValuesMsg(uint8_t *buffer) {
  uint8_t *p = buffer;
  *p++ = getEntityId();
  *p++ = mConfig.numberOfConfigItems;

  p += mConfig.reportHysteresis.writeConfigItemValue(p);
  p += mConfig.measureInterval.writeConfigItemValue(p);
  p += mConfig.reportInterval.writeConfigItemValue(p);
  p += mConfig.compensation.writeConfigItemValue(p);

  return p - buffer;
}

bool HumiditySensor::setConfigs(uint8_t numberOfConfigs,
                                const uint8_t *buffer) {
  if (numberOfConfigs > mConfig.numberOfConfigItems) {
    return false;
  }

  const uint8_t *p = buffer;
  while (numberOfConfigs-- > 0) {
    switch (*p) {
      case 0:
        p += mConfig.reportHysteresis.setConfigValue(p[0], &p[1]);
        break;
      case 1:
        p += mConfig.measureInterval.setConfigValue(p[0], &p[1]);
        break;
      case 2:
        p += mConfig.reportInterval.setConfigValue(p[0], &p[1]);
        break;
      case 3:
        p += mConfig.compensation.setConfigValue(p[0], &p[1]);
        break;
      default:
        return false;
    }
  }

  return true;
}
