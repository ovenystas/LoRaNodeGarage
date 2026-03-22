#pragma once

#include <stdint.h>

#include "Component.h"
#include "DHTReader.h"
#include "Number.h"
#include "Sensor.h"
#include "Util.h"

using TemperatureT = int16_t;  // Degree C

namespace TemperatureSensorConstants {
static const TemperatureT CONFIG_REPORT_HYSTERESIS_DEFAULT = 50;
static const uint16_t CONFIG_MEASURE_INTERVAL_DEFAULT = 60;
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
static const TemperatureT CONFIG_COMPENSATION_DEFAULT = 0;

static const char reportHysteresisName[] PROGMEM = "Report Hysteresis";
static const char measureIntervalName[] PROGMEM = "Measure Interval";
static const char reportIntervalName[] PROGMEM = "Report Interval";
static const char compensationName[] PROGMEM = "Compensation";
}  // namespace TemperatureSensorConstants

class TemperatureSensor : public IComponent {
 public:
  TemperatureSensor() = delete;

  TemperatureSensor(uint8_t entityId, const char *name, DHTReader &dhtReader)
      : mSensor{Sensor<TemperatureT>(
            entityId, name, SensorDeviceClass::TEMPERATURE, Unit::Type::C, 1)},

        mReportHysteresis{Number<TemperatureT>(
          entityId + 1, TemperatureSensorConstants::reportHysteresisName, NumberDeviceClass::TEMPERATURE, Unit::Type::C, 1,
          BaseComponent::Category::CONFIG,
          TemperatureSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT, 0, 100)},

        mMeasureInterval{Number<uint16_t>(
            entityId + 2, TemperatureSensorConstants::measureIntervalName, NumberDeviceClass::DURATION, Unit::Type::s, 0,
            BaseComponent::Category::CONFIG, TemperatureSensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT, 0,
            Util::ONE_HOUR_IN_SECONDS)},

        mReportInterval{Number<uint16_t>(
            entityId + 3, TemperatureSensorConstants::reportIntervalName, NumberDeviceClass::DURATION, Unit::Type::s, 0,
            BaseComponent::Category::CONFIG, TemperatureSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT, 0,
            Util::TWELVE_HOURS_IN_SECONDS)},

        mCompensation{Number<TemperatureT>(
            entityId + 4, TemperatureSensorConstants::compensationName, NumberDeviceClass::TEMPERATURE, Unit::Type::C, 1,
            BaseComponent::Category::CONFIG,
            TemperatureSensorConstants::CONFIG_COMPENSATION_DEFAULT, -100, 100)},

        mDhtReader{dhtReader} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigValueItems(ValueItemT* items, uint8_t length) const final;

  void loadConfigValues() final;
  
  uint8_t getDiscoveryItems(DiscoveryEntityItemT* item, uint8_t length) const final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }
  
  uint8_t getNumEntities() const final { return sNumItems; };

  void getValueItem(ValueItemT *item) const final {
    return mSensor.getValueItem(item);
  }

  bool setValueItem(const ValueItemT &item) final;

  bool isReportDue() const final { return mSensor.isReportDue(); }

  size_t printTo(Print &p) const final { return mSensor.printTo(p); };

  size_t printTo(Print &p, uint8_t service) const final {
    (void)p;
    (void)service;
    return 0;
  };

  void setReported() final { mSensor.setReported(); }

  bool update() final;

 private:
  static constexpr uint8_t sNumConfigItems = 4;
  static constexpr uint8_t sNumItems = 1 + sNumConfigItems;
  Sensor<TemperatureT> mSensor;
  Number<TemperatureT> mReportHysteresis;
  Number<uint16_t> mMeasureInterval;
  Number<uint16_t> mReportInterval;
  Number<TemperatureT> mCompensation;
  DHTReader &mDhtReader;
};
