#pragma once

#include "BinarySensor.h"
#include "Component.h"
#include "EeAdressMap.h"
#include "HeightSensor.h"
#include "PersistentNumberComponent.h"
#include "Util.h"

namespace PresenceBinarySensorConstants {
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
static const char reportIntervalName[] PROGMEM = "Report Interval";
}  // namespace PresenceBinarySensorConstants

class PresenceBinarySensor : public IComponent {
 public:
  PresenceBinarySensor() = delete;

  PresenceBinarySensor(uint8_t entityId, const char* name,
                       Sensor<HeightT>& heightSensor,
                       PersistentNumberComponent<HeightT>& lowLimit,
                       PersistentNumberComponent<HeightT>& highLimit,
                       PersistentNumberComponent<uint16_t>& minStableTime)
      : mBinarySensor{
            BinarySensor(entityId, name, BinarySensorDeviceClass::PRESENCE)},
        mHeightSensor{heightSensor},
        mLowLimit{lowLimit},
        mHighLimit{highLimit},
        mMinStableTime{minStableTime} {}

  void callService(uint8_t service) final { (void)service; }

  bool getConfigValue(ValueItemT& item, uint8_t index) const final;

  void loadConfigValues() final {};

  bool getDiscoveryEntity(DiscoveryEntityT& item) const final;

  uint8_t getEntityId() const final { return mBinarySensor.getEntityId(); }

  uint8_t getNumEntities() const final { return sNumItems; };

  uint8_t getNumConfigItems() const final { return sNumConfigItems; }

  void getValueItem(ValueItemT& item) const final {
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

  /**
   * @brief Get the number of entities this component provides
   * @return Number of entities (used for entity ID assignment in main.cpp)
   */
  static constexpr uint8_t getEntityCount() { return sNumItems; }

 private:
  static constexpr uint8_t sNumConfigItems = 3;
  static constexpr uint8_t sNumItems = 1 + sNumConfigItems;
  BinarySensor mBinarySensor;
  Sensor<HeightT>& mHeightSensor;
  PersistentNumberComponent<HeightT>& mLowLimit;
  PersistentNumberComponent<HeightT>& mHighLimit;
  PersistentNumberComponent<uint16_t>& mMinStableTime;
  uint32_t mLastChangedTime{};
  bool mStableState{};
};
