/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include "Component.h"

static const String mStateName[] = { "closed", "open", "opening", "closing" };
static const String mServiceName[] = { "open", "close", "stop", "toggle" };

/*
 *
 */
class Cover: public Component {
public:
  // From https://www.home-assistant.io/integrations/cover/ at 2021-03-21
  enum class DeviceClass {
    None,
    Awning,
    Blind,
    Curtain,
    Damper,
    Door,
    Garage,
    Gate,
    Shade,
    Shutter,
    Window
  };

  typedef enum {
    STATE_CLOSED,
    STATE_OPEN,
    STATE_OPENING,
    STATE_CLOSING
  } StateE;

  typedef enum {
    SERVICE_OPEN,
    SERVICE_CLOSE,
    SERVICE_STOP,
    SERVICE_TOGGLE
  } ServiceE;

  Cover(uint8_t entityId) :
      Component(entityId) {
  }
  virtual ~Cover() = default;

  virtual bool update() = 0;
  virtual StateE getState() const {
    return mState;
  }
  virtual const String& getStateName() const {
    return mStateName[mState];
  }
  virtual const String& getStateName(StateE state) const {
    return mStateName[state];
  }
  virtual const String& getServiceName(ServiceE service) const {
    return mServiceName[service];
  }
  Component::Type getComponent() const {
    return Component::Type::Cover;
  }
  virtual DeviceClass getDeviceClass() const {
    return DeviceClass::None;
  }
  virtual uint8_t* getDiscoveryMsg(uint8_t* buffer) {
    buffer[0] = getEntityId();
    buffer[1] = static_cast<uint8_t>(getComponent());
    buffer[2] = static_cast<uint8_t>(getDeviceClass());
    buffer[3] = 0;
    return buffer;
  }

protected:
  StateE mState = { };
};
