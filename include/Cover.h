#pragma once

#include <Printable.h>

#include "BaseComponent.h"
#include "Types.h"
#include "Util.h"

// From https://www.home-assistant.io/integrations/cover/ at 2021-03-21
enum class CoverDeviceClass : uint8_t {
  NONE,
  AWNING,
  BLIND,
  CURTAIN,
  DAMPER,
  DOOR,
  GARAGE,
  GATE,
  SHADE,
  SHUTTER,
  WINDOW
};

enum class CoverState : uint8_t { CLOSED, OPEN, OPENING, CLOSING };

enum class CoverService : uint8_t { OPEN, CLOSE, STOP, TOGGLE, UNKNOWN };

class Cover : public Printable {
 public:
  Cover() = delete;

  Cover(uint8_t entityId, const char* name,
        CoverDeviceClass deviceClass = CoverDeviceClass::NONE)
      : mBaseComponent{BaseComponent(entityId, name)},
        mDeviceClass{deviceClass} {}

  BaseComponent::Type getComponentType() const {
    return BaseComponent::Type::COVER;
  }

  CoverDeviceClass getDeviceClass() const { return mDeviceClass; }

  void getDiscoveryEntityItem(DiscoveryEntityItemT* item) const;

  uint8_t getEntityId() const { return mBaseComponent.getEntityId(); }

  CoverState getState() const { return mState; }

  void getValueItem(ValueItemT* item) const;

  bool isDiffLastReportedState() const { return mState != mLastReportedState; }

  bool isReportDue() const { return mBaseComponent.isReportDue(); }

  size_t printTo(Print& p) const final;

  size_t printTo(Print& p, uint8_t service) const;

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

  const __FlashStringHelper* getStateName() const;

  const __FlashStringHelper* getServiceName(CoverService service) const;

 private:
  BaseComponent mBaseComponent;
  const CoverDeviceClass mDeviceClass;
  CoverState mState{CoverState::CLOSED};
  CoverState mLastReportedState{CoverState::CLOSED};
};
