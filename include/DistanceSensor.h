#pragma once

#include <NewPing.h>

#include "Component.h"
#include "Number.h"
#include "Sensor.h"
#include "Util.h"

using DistanceT = int16_t;  // cm

namespace DistanceSensorConstants {
constexpr int16_t CONFIG_REPORT_HYSTERESIS_DEFAULT = 10;
constexpr uint16_t CONFIG_MEASURE_INTERVAL_DEFAULT = 60;
constexpr uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
}  // namespace DistanceSensorConstants

static const char reportHysteresisName[] PROGMEM = "Report Hysteresis";
static const char measureIntervalName[] PROGMEM = "Measure Interval";
static const char reportIntervalName[] PROGMEM = "Report Interval";

class DistanceSensor : public IComponent {
 public:
  DistanceSensor() = delete;

  DistanceSensor(uint8_t entityId, const char* name, NewPing& sonar)
      : mSensor{Sensor<DistanceT>(entityId, name, SensorDeviceClass::DISTANCE,
                                  Unit::Type::cm)},

        mReportHysteresis{Number<int16_t>(
            entityId + 1, reportHysteresisName, NumberDeviceClass::DISTANCE, Unit::Type::cm,
            0, BaseComponent::Category::CONFIG, DistanceSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT, 0,
            MAX_SENSOR_DISTANCE)},

        mMeasureInterval{Number<uint16_t>(
            entityId + 2, measureIntervalName, NumberDeviceClass::DURATION, Unit::Type::s, 0,
            BaseComponent::Category::CONFIG, DistanceSensorConstants::CONFIG_MEASURE_INTERVAL_DEFAULT, 0,
            Util::ONE_HOUR_IN_SECONDS)},

        mReportInterval{Number<uint16_t>(
            entityId + 3, reportIntervalName, NumberDeviceClass::DURATION, Unit::Type::s, 0,
            BaseComponent::Category::CONFIG, DistanceSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT, 0,
            Util::TWELVE_HOURS_IN_SECONDS)},

        mSonar{sonar} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigValueItems(ValueItemT* items, uint8_t length) const final;

  void loadConfigValues() final;

  uint8_t getDiscoveryItems(DiscoveryEntityItemT* item, uint8_t length) const final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  uint8_t getNumEntities() const final { return sNumItems; };

  Sensor<DistanceT>& getSensor() { return mSensor; }

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
  static constexpr uint8_t sNumConfigItems = 3;
  static constexpr uint8_t sNumItems = 1 + sNumConfigItems;
  Sensor<DistanceT> mSensor;
  Number<int16_t> mReportHysteresis;
  Number<uint16_t> mMeasureInterval;
  Number<uint16_t> mReportInterval;
  NewPing& mSonar;
};
