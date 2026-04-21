#pragma once

#include <stdint.h>

#include "Component.h"
#include "DistanceSensor.h"
#include "EeAdressMap.h"
#include "NewPing.h"
#include "PersistentNumberComponent.h"
#include "Sensor.h"
#include "Unit.h"
#include "Util.h"

using HeightT = int16_t;  // cm

namespace HeightSensorConstants {
static const int16_t CONFIG_REPORT_HYSTERESIS_DEFAULT = 10;
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
}  // namespace HeightSensorConstants

class HeightSensor : public IComponent {
 public:
  HeightSensor() = delete;

  HeightSensor(uint8_t entityId, const char* name,
               Sensor<DistanceT>& distanceSensor,
               PersistentNumberComponent<uint16_t>& stableTime,
               PersistentNumberComponent<HeightT>& zeroValue)
      : mSensor{Sensor<HeightT>(entityId, name, SensorDeviceClass::DISTANCE,
                                Unit::Type::cm)},
        mDistanceSensor{distanceSensor},
        mStableTime{stableTime},
        mZeroValue{zeroValue} {}

  void callService(uint8_t service) final { (void)service; }

  bool getConfigValue(ValueItemT& item, uint8_t index) const final;

  void loadConfigValues() final {};

  bool getDiscoveryEntity(DiscoveryEntityT& item) const final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  uint8_t getNumEntities() const final { return sNumItems; };

  uint8_t getNumConfigItems() const final { return sNumConfigItems; }

  Sensor<HeightT>& getSensor() { return mSensor; }

  IComponent* getComponentByEntityId(uint8_t entityId);

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

  /**
   * @brief Get the number of entities this component provides
   * @return Number of entities (used for entity ID assignment in main.cpp)
   */
  static constexpr uint8_t getEntityCount() { return sNumItems; }

 private:
  static constexpr uint8_t sNumConfigItems = 2;
  static constexpr uint8_t sNumItems = 1 + sNumConfigItems;
  Sensor<HeightT> mSensor;
  Sensor<DistanceT>& mDistanceSensor;
  PersistentNumberComponent<uint16_t>& mStableTime;
  PersistentNumberComponent<HeightT>& mZeroValue;
};
