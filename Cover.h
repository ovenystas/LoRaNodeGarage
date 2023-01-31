#pragma once

#include <Stream.h>

#include "Component.h"
#include "Util.h"

static const char CoverStateName[][8] = {"closed", "open", "opening",
                                         "closing"};

static const char CoverServiceName[][7] = {"open", "close", "stop", "toggle"};

// From https://www.home-assistant.io/integrations/cover/ at 2021-03-21
enum class CoverDeviceClass {
  none,
  awning,
  blind,
  curtain,
  damper,
  door,
  garage,
  gate,
  shade,
  shutter,
  window
};

enum class CoverState { closed, open, opening, closing };

enum class CoverService { open, close, stop, toggle };

class ICover : public virtual IComponent {
 public:
  virtual ~ICover() = default;

  virtual void callService(CoverService service) = 0;

  virtual CoverState getState() const = 0;

  virtual const char* getStateName() const = 0;

  virtual const char* getStateName(CoverState state) const = 0;

  virtual const char* getServiceName(CoverService service) const = 0;

  virtual IComponent::Type getComponentType() const = 0;

  virtual CoverDeviceClass getDeviceClass() const = 0;

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) = 0;

  virtual uint8_t getValueMsg(uint8_t* buffer) = 0;

  virtual bool isDiffLastReportedState() const = 0;

  virtual void setState(CoverState state) = 0;

 private:
  CoverState mState = {CoverState::closed};
  CoverState mLastReportedState = {CoverState::closed};
};

class Cover : public virtual ICover, public Component {
 public:
  Cover(uint8_t entityId, const char* name,
        CoverDeviceClass deviceClass = CoverDeviceClass::none)
      : Component(entityId, name), mDeviceClass{deviceClass} {}

  bool hasService() final { return true; }

  // void callService(uint8_t service) final {
  //   callService(static_cast<CoverService>(service));
  // }

  CoverState getState() const final { return mState; }

  const char* getStateName() const final {
    return CoverStateName[static_cast<uint8_t>(mState)];
  }

  const char* getStateName(CoverState state) const final {
    return CoverStateName[static_cast<uint8_t>(state)];
  }

  const char* getServiceName(CoverService service) const final {
    return CoverServiceName[static_cast<uint8_t>(service)];
  }

  IComponent::Type getComponentType() const final {
    return IComponent::Type::cover;
  }

  CoverDeviceClass getDeviceClass() const final { return mDeviceClass; }

  uint8_t getDiscoveryMsg(uint8_t* buffer) override;

  uint8_t getValueMsg(uint8_t* buffer) final;

  void setReported() final {
    Component::setReported();
    mLastReportedState = mState;
  }

  bool isDiffLastReportedState() const final {
    return mState != mLastReportedState;
  }

  void print(Stream& stream) final;

  void print(Stream& stream, uint8_t service) final;

  void setState(CoverState state) final { mState = state; }

 private:
  const CoverDeviceClass mDeviceClass = {CoverDeviceClass::none};
  CoverState mState = {CoverState::closed};
  CoverState mLastReportedState = {CoverState::closed};
};
