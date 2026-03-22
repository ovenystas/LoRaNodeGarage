#pragma once

#include <stdint.h>

#include "Component.h"
#include "DHTReader.h"
#include "Number.h"
#include "Sensor.h"
#include "Util.h"

using HumidityT = uint8_t;  // %

namespace HumiditySensorConstants {
static const HumidityT CONFIG_REPORT_HYSTERESIS_DEFAULT = 10;
static const uint16_t CONFIG_MEASURE_INTERVAL_DEFAULT = 60;
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
static const HumidityT CONFIG_COMPENSATION_DEFAULT = 0;

static const char reportHysteresisName[] PROGMEM = "Report Hysteresis";
static const char measureIntervalName[] PROGMEM = "Measure Interval";
static const char reportIntervalName[] PROGMEM = "Report Interval";
static const char compensationName[] PROGMEM = "Compensation";
}  // namespace HumiditySensorConstants

class HumiditySensor : public IComponent {
 public:
  HumiditySensor() = delete;

  HumiditySensor(uint8_t entityId, const char* name, DHTReader& dhtReader)
      : mSensor{Sensor<HumidityT>(entityId, name, SensorDeviceClass::HUMIDITY,
                                  Unit::Type::percent)},

        mReportHysteresis{Number<HumidityT>(
          entityId + 1, HumiditySensorConstants::reportHysteresisName, NumberDeviceClass::HUMIDITY, Unit::Type::percent, 1,
          BaseComponent::Category::CONFIG,
          HumiditySensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT, 0, 100)},

        mMeasureInterval{Number<uint16_t>(
            entityId + 2, HumiditySensorConstants::measureIntervalName, NumberDeviceClass::DURATION, Unit::Type::s, 0,
            BaseComponent::Category::CONFIG, HumiditySensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT, 0,
            Util::ONE_HOUR_IN_SECONDS)},

        mReportInterval{Number<uint16_t>(
            entityId + 3, HumiditySensorConstants::reportIntervalName, NumberDeviceClass::DURATION, Unit::Type::s, 0,
            BaseComponent::Category::CONFIG, HumiditySensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT, 0,
            Util::TWELVE_HOURS_IN_SECONDS)},

        mCompensation{Number<HumidityT>(
            entityId + 4, HumiditySensorConstants::compensationName, NumberDeviceClass::HUMIDITY, Unit::Type::percent, 1,
            BaseComponent::Category::CONFIG,
            HumiditySensorConstants::CONFIG_COMPENSATION_DEFAULT, -100, 100)},

        mDhtReader{dhtReader} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigValueItems(ValueItemT* items, uint8_t length) const final;

  void loadConfigValues() final;
  
  uint8_t getDiscoveryItems(DiscoveryEntityItemT* item, uint8_t length) const final;
  
  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  uint8_t getNumEntities() const final { return sNumItems; };

  void getValueItem(ValueItemT* item) const final {
    return mSensor.getValueItem(item);
  }

  bool setValueItem(const ValueItemT &item) final;

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
  static constexpr uint8_t sNumConfigItems = 4;
  static constexpr uint8_t sNumItems = 1 + sNumConfigItems;
  Sensor<HumidityT> mSensor;
  Number<HumidityT> mReportHysteresis;
  Number<uint16_t> mMeasureInterval;
  Number<uint16_t> mReportInterval;
  Number<HumidityT> mCompensation;
  DHTReader& mDhtReader;
};
