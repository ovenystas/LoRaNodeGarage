#pragma once

#include <Printable.h>
#include <WString.h>

#include "BaseComponent.h"
#include "Types.h"
#include "Unit.h"
#include "Util.h"

// From https://www.home-assistant.io/integrations/binary_sensor/ at
// 2021-03-21
enum class BinarySensorDeviceClass : uint8_t {
  NONE,
  BATTERY,
  BATTERY_CHARGING,
  COLD,
  CONNECTIVITY,
  DOOR,
  GARAGE_DOOR,
  GAS,
  HEAT,
  LIGHT,
  LOCK,
  MOISTURE,
  MOTION,
  MOVING,
  OCCUPANCY,
  OPENING,
  PLUG,
  POWER,
  PRESENCE,
  PROBLEM,
  SAFETY,
  SMOKE,
  SOUND,
  VIBRATION,
  WINDOW
};

class BinarySensor : public Printable {
 public:
  BinarySensor() = delete;

  BinarySensor(
      uint8_t entityId, const char* name,
      BinarySensorDeviceClass deviceClass = BinarySensorDeviceClass::NONE,
      Unit::Type unitType = Unit::Type::none)
      : mBaseComponent{BaseComponent(entityId, name, BaseComponent::Category::DIAGNOSTIC)},
        mDeviceClass{deviceClass},
        mUnit{Unit(unitType)} {}

  BaseComponent::Type getComponentType() const {
    return BaseComponent::Type::BINARY_SENSOR;
  }

  BinarySensorDeviceClass getDeviceClass() const { return mDeviceClass; }

  BaseComponent::Category getCategory() const { return mBaseComponent.getCategory(); }

  void getDiscoveryEntityItem(DiscoveryEntityItemT* item) const;

  uint8_t getEntityId() const { return mBaseComponent.getEntityId(); }

  bool getState() const { return mState; }

  const __FlashStringHelper* getStateName() const;

  void getValueItem(ValueItemT* item) const;

  bool isDiffLastReportedState() const { return mState != mLastReportedState; }

  bool isReportDue() const { return mBaseComponent.isReportDue(); }

  size_t printTo(Print& p) const final;

  void setIsReportDue(bool isDue) { mBaseComponent.setIsReportDue(isDue); }

  void setReported() {
    mBaseComponent.setReported();
    mLastReportedState = mState;
  }

  void setState(bool state) { mState = state; }

  uint32_t timeSinceLastReport() const {
    return mBaseComponent.timeSinceLastReport();
  }

 private:
  BaseComponent mBaseComponent;
  const BinarySensorDeviceClass mDeviceClass;
  const Unit mUnit;
  bool mState{};
  bool mLastReportedState{};
};
