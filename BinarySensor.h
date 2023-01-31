#pragma once

#include <Stream.h>
#include <WString.h>

#include "Component.h"
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

class IBinarySensor : public virtual IComponent {
 protected:
  virtual ~IBinarySensor() = default;

  virtual bool getState() const = 0;

  virtual const __FlashStringHelper* getStateName() = 0;

  virtual IComponent::Type getComponentType() const = 0;

  virtual BinarySensorDeviceClass getDeviceClass() const = 0;

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) = 0;

  virtual uint8_t getValueMsg(uint8_t* buffer) = 0;

  virtual bool isDiffLastReportedState() const = 0;

  virtual void setState(bool state) = 0;
};

class BinarySensor : public virtual IBinarySensor, public Component {
 public:
  BinarySensor(
      uint8_t entityId, const char* name,
      BinarySensorDeviceClass deviceClass = BinarySensorDeviceClass::none)
      : Component(entityId, name), mDeviceClass{deviceClass} {}

  bool hasService() final { return false; }

  void print(Stream& stream, uint8_t service) final {
    (void)stream;
    (void)service;
  }

  bool getState() const final { return mState; }

  const __FlashStringHelper* getStateName() final;

  Component::Type getComponentType() const final {
    return Component::Type::binarySensor;
  }

  BinarySensorDeviceClass getDeviceClass() const final { return mDeviceClass; }

  uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  uint8_t getValueMsg(uint8_t* buffer) final;

  void setReported() final {
    Component::setReported();
    mLastReportedState = mState;
  }

  bool isDiffLastReportedState() const final {
    return mState != mLastReportedState;
  }

  void print(Stream& stream) final;

  void setState(bool state) final { mState = state; }

 private:
  const BinarySensorDeviceClass mDeviceClass = {BinarySensorDeviceClass::none};
  bool mState = {};
  bool mLastReportedState = {};
};
