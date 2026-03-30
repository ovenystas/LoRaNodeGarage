#pragma once

#include "BinarySensor.h"
#include "Component.h"
#include "HeightSensor.h"
#include "Number.h"
#include "Util.h"

namespace PresenceBinarySensorConstants {
static const HeightT CONFIG_LOW_LIMIT_DEFAULT = 180;
static const HeightT CONFIG_HIGH_LIMIT_DEFAULT = 200;
static const uint16_t CONFIG_MIN_STABLE_TIME_DEFAULT = 10000;
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;

static const char lowLimitName[] PROGMEM = "Low Limit";
static const char highLimitName[] PROGMEM = "High Limit";
static const char minStableTimeName[] PROGMEM = "Min Stable Time";
static const char reportIntervalName[] PROGMEM = "Report Interval";
}  // namespace PresenceBinarySensorConstants

class PresenceBinarySensor : public IComponent {
 public:
  PresenceBinarySensor() = delete;

  PresenceBinarySensor(uint8_t entityId, const char* name,
                       Sensor<HeightT>& heightSensor)
      : mBinarySensor{
            BinarySensor(entityId, name, BinarySensorDeviceClass::PRESENCE)},

        mLowLimit{Number<HeightT>(
            0, PresenceBinarySensorConstants::lowLimitName,
            NumberDeviceClass::DISTANCE, Unit::Type::cm, 0,
            BaseComponent::Category::CONFIG,
            PresenceBinarySensorConstants::CONFIG_LOW_LIMIT_DEFAULT, 0,
            MAX_SENSOR_DISTANCE)},

        mHighLimit{Number<HeightT>(
            1, PresenceBinarySensorConstants::highLimitName,
            NumberDeviceClass::DISTANCE, Unit::Type::cm, 0,
            BaseComponent::Category::CONFIG,
            PresenceBinarySensorConstants::CONFIG_HIGH_LIMIT_DEFAULT, 0,
            MAX_SENSOR_DISTANCE)},

        mMinStableTime{Number<uint16_t>(
            2, PresenceBinarySensorConstants::minStableTimeName,
            NumberDeviceClass::DURATION, Unit::Type::ms, 0,
            BaseComponent::Category::CONFIG,
            PresenceBinarySensorConstants::CONFIG_MIN_STABLE_TIME_DEFAULT, 0,
            Util::MS_PER_MINUTE)},

        mHeightSensor{heightSensor} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigValueItems(ValueItemT* items, uint8_t length) const final;

  void loadConfigValues() final;

  uint8_t getDiscoveryItems(DiscoveryEntityItemT* item,
                            uint8_t length) const final;

  uint8_t getEntityId() const final { return mBinarySensor.getEntityId(); }

  uint8_t getNumEntities() const final { return sNumItems; };

  void getValueItem(ValueItemT* item) const final {
    return mBinarySensor.getValueItem(item);
  }

  bool setValueItem(const ValueItemT& item) final;

  bool isReportDue() const final { return mBinarySensor.isReportDue(); }

  size_t printTo(Print& p) const final { return mBinarySensor.printTo(p); };

  size_t printTo(Print& p, uint8_t service) const final {
    (void)p;
    (void)service;
    return 0;
  };

  void setReported() final { mBinarySensor.setReported(); }

  bool update() final;

 private:
  static constexpr uint8_t sNumConfigItems = 3;
  static constexpr uint8_t sNumItems = 1 + sNumConfigItems;
  BinarySensor mBinarySensor;
  Number<HeightT> mLowLimit;
  Number<HeightT> mHighLimit;
  Number<uint16_t> mMinStableTime;
  Sensor<HeightT>& mHeightSensor;
  uint32_t mLastChangedTime{};
  bool mStableState{};
};
