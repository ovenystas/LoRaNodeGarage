#pragma once

#include <Stream.h>
#include <WString.h>

#include "BaseComponent.h"
#include "Unit.h"
#include "Util.h"

// From https://www.home-assistant.io/integrations/binary_sensor/ at
// 2021-03-21
enum class BinarySensorDeviceClass {
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

class BinarySensor {
 public:
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

  uint8_t getDiscoveryMsg(uint8_t* buffer);

  uint8_t getEntityId() const { return mBaseComponent.getEntityId(); }

  bool getState() const { return mState; }

  const __FlashStringHelper* getStateName();

  uint8_t getValueMsg(uint8_t* buffer);

  bool isDiffLastReportedState() const { return mState != mLastReportedState; }

  void print(Stream& stream);

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
  const BinarySensorDeviceClass mDeviceClass = {BinarySensorDeviceClass::none};
  const Unit mUnit;
  bool mState = {};
  bool mLastReportedState = {};
};
