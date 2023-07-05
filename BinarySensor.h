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

class IBinarySensor : public virtual IBaseComponent {
 protected:
  virtual ~IBinarySensor() = default;

  virtual bool getState() const = 0;

  virtual const __FlashStringHelper* getStateName() = 0;

  virtual IBaseComponent::Type getComponentType() const = 0;

  virtual BinarySensorDeviceClass getDeviceClass() const = 0;

  virtual bool isDiffLastReportedState() const = 0;

  virtual void setState(bool state) = 0;
};

class BinarySensor : public virtual IBinarySensor, public BaseComponent {
 public:
  BinarySensor(
      uint8_t entityId, const char* name,
      BinarySensorDeviceClass deviceClass = BinarySensorDeviceClass::none,
      Unit::Type unitType = Unit::Type::none)
      : BaseComponent(entityId, name),
        mDeviceClass{deviceClass},
        mUnit{Unit(unitType)} {}

  bool hasService() final { return false; }

  void print(Stream& stream, uint8_t service) final {
    (void)stream;
    (void)service;
  }

  bool getState() const final { return mState; }

  const __FlashStringHelper* getStateName() final;

  BaseComponent::Type getComponentType() const final {
    return BaseComponent::Type::binarySensor;
  }

  BinarySensorDeviceClass getDeviceClass() const final { return mDeviceClass; }

  uint8_t getDiscoveryMsg(uint8_t* buffer) override;

  uint8_t getValueMsg(uint8_t* buffer) final;

  void setReported() final {
    BaseComponent::setReported();
    mLastReportedState = mState;
  }

  bool isDiffLastReportedState() const final {
    return mState != mLastReportedState;
  }

  void print(Stream& stream) final;

  void setState(bool state) final { mState = state; }

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) override {
    (void)buffer;
    return false;
  }

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) override {
    (void)numberOfConfigs;
    (void)buffer;
    return false;
  }

  bool update() override { return false; }

 private:
  const BinarySensorDeviceClass mDeviceClass = {BinarySensorDeviceClass::none};
  const Unit mUnit;
  bool mState = {};
  bool mLastReportedState = {};
};
