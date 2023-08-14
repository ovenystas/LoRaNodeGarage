#pragma once

#include <Print.h>

#include "BaseComponent.h"
#include "Types.h"
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
  Cover() = delete;

  Cover(uint8_t entityId, const char* name,
        CoverDeviceClass deviceClass = CoverDeviceClass::none)
      : mBaseComponent{BaseComponent(entityId, name)},
        mDeviceClass{deviceClass} {}

  BaseComponent::Type getComponentType() const {
    return BaseComponent::Type::cover;
  }

  CoverDeviceClass getDeviceClass() const { return mDeviceClass; }

  void getDiscoveryEntityItem(DiscoveryEntityItemT* item) const;

  uint8_t getEntityId() const { return mBaseComponent.getEntityId(); }

  const char* getServiceName(CoverService service) const;

  CoverState getState() const { return mState; }

  const char* getStateName() const;

  const char* getStateName(CoverState state) const;

  void getValueItem(ValueItemT* item) const;

  bool isDiffLastReportedState() const { return mState != mLastReportedState; }

  bool isReportDue() const { return mBaseComponent.isReportDue(); }

  size_t print(Print& printer) const;

  size_t print(Print& printer, uint8_t service) const;

  CoverService serviceDecode(uint8_t service) const;

  void setIsReportDue(bool isDue) { mBaseComponent.setIsReportDue(isDue); }

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
  const CoverDeviceClass mDeviceClass;
  CoverState mState{CoverState::closed};
  CoverState mLastReportedState{CoverState::closed};
};
