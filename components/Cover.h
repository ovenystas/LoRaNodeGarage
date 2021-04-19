/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include "Component.h"

#include <Stream.h>

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

  virtual bool update() = 0;

  virtual void callService(const Service service) = 0;

  virtual State getState() const {
    return mState;
  }

  virtual const String& getStateName() const {
    return CoverStateName[static_cast<uint8_t>(mState)];
  }

  virtual const String& getStateName(State state) const {
    return CoverStateName[static_cast<uint8_t>(state)];
  }

  virtual const String& getServiceName(Service service) const {
    return CoverServiceName[static_cast<uint8_t>(service)];
  }

  Component::Type getComponent() const {
    return Component::Type::Cover;
  }

  virtual DeviceClass getDeviceClass() const {
    return DeviceClass::none;
  }

  virtual uint8_t* getDiscoveryMsg(uint8_t* buffer);

  void print(Stream& stream);

protected:
  State mState = { };
};
