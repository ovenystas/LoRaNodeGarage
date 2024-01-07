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
  none,
  battery,
  batteryCharging,
  cold,
  connectivity,
  door,
  garageDoor,
  gas,
  heat,
  light,
  lock,
  moisture,
  motion,
  moving,
  occupancy,
  opening,
  plug,
  power,
  presence,
  problem,
  safety,
  smoke,
  sound,
  vibration,
  window
};

class BinarySensor : public Printable {
 public:
  BinarySensor() = delete;

  BinarySensor(
      uint8_t entityId, const char* name,
      BinarySensorDeviceClass deviceClass = BinarySensorDeviceClass::none,
      Unit::Type unitType = Unit::Type::none)
      : mBaseComponent{BaseComponent(entityId, name)},
        mDeviceClass{deviceClass},
        mUnit{Unit(unitType)} {}

  BaseComponent::Type getComponentType() const {
    return BaseComponent::Type::binarySensor;
  }

  BinarySensorDeviceClass getDeviceClass() const { return mDeviceClass; }

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
