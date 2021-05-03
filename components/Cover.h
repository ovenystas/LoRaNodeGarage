/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <Stream.h>

#include "Component.h"
#include "Util.h"

static const String CoverStateName[] = { "closed", "open", "opening", "closing" };
static const String CoverServiceName[] = { "open", "close", "stop", "toggle" };

/*
 *
 */
class Cover: public Component {
public:
  // From https://www.home-assistant.io/integrations/cover/ at 2021-03-21
  enum class DeviceClass {
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

  enum class State {
    closed,
    open,
    opening,
    closing
  };

  enum class Service {
    open,
    close,
    stop,
    toggle
  };

  Cover(uint8_t entityId, const char* name) :
      Component(entityId, name) {
  }

  virtual ~Cover() = default;

  virtual bool hasService() final {
    return true;
  }

  virtual void callService(Service service) = 0;

  virtual void callService(uint8_t service) override {
    callService(static_cast<Service>(service));
  }

  inline State getState() const {
    return mState;
  }

  const String& getStateName() const {
    return CoverStateName[static_cast<uint8_t>(mState)];
  }

  const String& getStateName(State state) const {
    return CoverStateName[static_cast<uint8_t>(state)];
  }

  const String& getServiceName(Service service) const {
    return CoverServiceName[static_cast<uint8_t>(service)];
  }

  Component::Type getComponent() const {
    return Component::Type::cover;
  }

  virtual DeviceClass getDeviceClass() const {
    return DeviceClass::none;
  }

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) override;

  virtual uint8_t getValueMsg(uint8_t* buffer) final;

  virtual void setReported() final {
    mLastReportTime = seconds();
    mLastReportedState = mState;
  }

  virtual void print(Stream& stream) final;

  virtual void print(Stream& stream, uint8_t service) final;

protected:
  inline void setState(State state) {
    mState = state;
  }

private:
  State mState = { };
  State mLastReportedState = { };
  uint32_t mLastReportTime = { }; // s
};
