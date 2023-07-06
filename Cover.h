#pragma once

#include <Stream.h>

#include "BaseComponent.h"
#include "Util.h"

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

enum class CoverService { open, close, stop, toggle, unknown };

class Cover {
 public:
  Cover(uint8_t entityId, const char* name,
        CoverDeviceClass deviceClass = CoverDeviceClass::none)
      : mBaseComponent{BaseComponent(entityId, name)},
        mDeviceClass{deviceClass} {}

  BaseComponent::Type getComponentType() const {
    return BaseComponent::Type::cover;
  }

  CoverDeviceClass getDeviceClass() const { return mDeviceClass; }

  uint8_t getDiscoveryMsg(uint8_t* buffer);

  uint8_t getEntityId() const { return mBaseComponent.getEntityId(); }

  const char* getServiceName(CoverService service) const;

  CoverState getState() const { return mState; }

  const char* getStateName() const;

  const char* getStateName(CoverState state) const;

  uint8_t getValueMsg(uint8_t* buffer);

  bool isDiffLastReportedState() const { return mState != mLastReportedState; }

  void print(Stream& stream);

  void print(Stream& stream, uint8_t service);

  CoverService serviceDecode(uint8_t service);

  void setReported() {
    mBaseComponent.setReported();
    mLastReportedState = mState;
  }

  void setState(CoverState state) { mState = state; }

  uint32_t timeSinceLastReport() const {
    return mBaseComponent.timeSinceLastReport();
  }

 private:
  BaseComponent mBaseComponent;
  const CoverDeviceClass mDeviceClass = {CoverDeviceClass::none};
  CoverState mState = {CoverState::closed};
  CoverState mLastReportedState = {CoverState::closed};
};
