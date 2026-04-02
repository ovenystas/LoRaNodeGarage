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

class DistanceSensor : public IComponent {
 public:
  DistanceSensor() = delete;

  DistanceSensor(uint8_t entityId, const char* name, NewPing& sonar)
      : mSensor{Sensor<DistanceT>(entityId, name, SensorDeviceClass::DISTANCE,
                                  Unit::Type::cm)},
        mSonar{sonar} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigValueItems(ValueItemT* items, uint8_t length) const final;

  void loadConfigValues() final;

  uint8_t getDiscoveryItems(DiscoveryEntityItemT* item,
                            uint8_t length) const final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  uint8_t getNumEntities() const final { return sNumItems; };

  Sensor<DistanceT>& getSensor() { return mSensor; }

  void getValueItem(ValueItemT* item) const final {
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
  static constexpr uint8_t sNumConfigItems = 3;
  static constexpr uint8_t sNumItems = 1 + sNumConfigItems;
  Sensor<DistanceT> mSensor;
  NewPing& mSonar;
};
