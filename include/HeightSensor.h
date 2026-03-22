#pragma once

#include <stdint.h>

#include "Component.h"
#include "DistanceSensor.h"
#include "NewPing.h"
#include "Number.h"
#include "Sensor.h"
#include "Unit.h"
#include "Util.h"

using HeightT = int16_t;  // cm

namespace HeightSensorConstants {
static const int16_t CONFIG_REPORT_HYSTERESIS_DEFAULT = 10;
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
static const uint16_t CONFIG_STABLE_TIME_DEFAULT = 5000;
static const HeightT CONFIG_ZERO_VALUE_DEFAULT = 60;

static const char reportHysteresisName[] PROGMEM = "Report Hysteresis";
static const char reportIntervalName[] PROGMEM = "Report Interval";
static const char stableTimeName[] PROGMEM = "Stable Time";
static const char zeroValueName[] PROGMEM = "Zero Value";
}  // namespace HeightSensorConstants

class HeightSensor : public IComponent {
 public:
  HeightSensor() = delete;

  HeightSensor(uint8_t entityId, const char* name,
               Sensor<DistanceT>& distanceSensor)
      : mSensor{Sensor<HeightT>(entityId, name, SensorDeviceClass::DISTANCE,
                                Unit::Type::cm)},

        mReportHysteresis{Number<HeightT>(
            entityId + 1, HeightSensorConstants::reportHysteresisName, NumberDeviceClass::DISTANCE, Unit::Type::cm, 0,
            BaseComponent::Category::CONFIG, HeightSensorConstants::CONFIG_REPORT_HYSTERESIS_DEFAULT, 0,
            MAX_SENSOR_DISTANCE)},

        mReportInterval{Number<uint16_t>(
            entityId + 2, HeightSensorConstants::reportIntervalName, NumberDeviceClass::DURATION, Unit::Type::s, 0,
            BaseComponent::Category::CONFIG, HeightSensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT, 0,
            Util::ONE_HOUR_IN_SECONDS)},

        mStableTime{Number<uint16_t>(
            entityId + 3, HeightSensorConstants::stableTimeName, NumberDeviceClass::DURATION, Unit::Type::ms, 0,
            BaseComponent::Category::CONFIG, HeightSensorConstants::CONFIG_STABLE_TIME_DEFAULT, 0,
            Util::ONE_MINUTE_IN_MILLISECONDS)},

        mZeroValue{Number<HeightT>(
            entityId + 4, HeightSensorConstants::zeroValueName, NumberDeviceClass::DISTANCE, Unit::Type::cm, 0,
            BaseComponent::Category::CONFIG, HeightSensorConstants::CONFIG_ZERO_VALUE_DEFAULT, -MAX_SENSOR_DISTANCE,
            MAX_SENSOR_DISTANCE)},

        mDistanceSensor{distanceSensor} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigValueItems(ValueItemT* items, uint8_t length) const final;

  void loadConfigValues() final;
  
  uint8_t getDiscoveryItems(DiscoveryEntityItemT* item, uint8_t length) const final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }
  
  uint8_t getNumEntities() const final { return sNumItems; };

  Sensor<HeightT>& getSensor() { return mSensor; }

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
  Sensor<HeightT> mSensor;
  Number<HeightT> mReportHysteresis;
  Number<uint16_t> mReportInterval;
  Number<uint16_t> mStableTime;
  Number<HeightT> mZeroValue;
  Sensor<DistanceT>& mDistanceSensor;
};
