/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>

static const String mStateName[] = { "closed", "open", "opening", "closing" };
static const String mServiceName[] = { "open", "close", "stop", "toggle" };

/*
 *
 */
class Cover {
public:
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

  Cover() = default;
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

protected:
  StateE mState = { };
};
