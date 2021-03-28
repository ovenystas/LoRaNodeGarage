/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>

#include "components/Cover.h"


class GarageCover : public Cover {
public:
  GarageCover() = delete;
  GarageCover(uint8_t entityId, uint8_t pinClosed, uint8_t pinOpen) :
    Cover(entityId), mPinClosed { pinClosed }, mPinOpen { pinOpen } {
  }
  bool update();
  void callService(ServiceE service);
  virtual DeviceClass getDeviceClass() const override {
    return DeviceClass::Garage;
  }

  // Cover
private:
  void openCover();
  void closeCover();
  void stopCover();
  void toggleCover();

  uint8_t mPinClosed;
  uint8_t mPinOpen;
};
