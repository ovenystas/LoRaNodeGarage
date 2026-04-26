#pragma once

#include <stdint.h>

#include "AHTReader.h"
#include "Component.h"
#include "Number.h"
#include "Sensor.h"
#include "Util.h"

using HumidityT = uint8_t;  // %

namespace HumiditySensorConstants {
static const HumidityT CONFIG_REPORT_HYSTERESIS_DEFAULT = 10;
static const uint16_t CONFIG_MEASURE_INTERVAL_DEFAULT = 60;
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
static const int8_t CONFIG_COMPENSATION_DEFAULT = 0;
}  // namespace HumiditySensorConstants

class HumiditySensor : public IComponent {
 public:
  HumiditySensor() = delete;

  HumiditySensor(uint8_t entityId, const char* name, AHTReader& ahtReader)
      : mSensor{Sensor<HumidityT>(entityId, name, SensorDeviceClass::HUMIDITY,
                                  Unit::Type::percent, 0, 0, 0, 100)},
        mAhtReader{ahtReader} {}

  void callService(uint8_t service) final { (void)service; }

  void loadConfigValues() final {};

  bool getDiscoveryEntity(DiscoveryEntityT& item) const final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  void getValueItem(ValueItemT& item) const final {
    return mSensor.getValueItem(item);
  }

  bool setValueItem(const ValueItemT& item) final;

  bool isReportDue() const final { return mSensor.isReportDue(); }

  size_t printTo(Print& p) const final { return mSensor.printTo(p); };

  size_t printTo(Print& p, uint8_t service) const final {
    (void)p;
    (void)service;
    return 0;
  };

  void setReported() final { mSensor.setReported(); }

  bool update() final;

 private:
  Sensor<HumidityT> mSensor;
  AHTReader& mAhtReader;
};
