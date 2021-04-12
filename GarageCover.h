/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>

#include "components/Cover.h"

class GarageCover: public Cover {
public:
  GarageCover() = delete;

  GarageCover(uint8_t entityId, const char* name, uint8_t pinClosed, uint8_t pinOpen) :
      Cover(entityId, name), mPinClosed { pinClosed }, mPinOpen { pinOpen } {
    pinMode(pinClosed, INPUT_PULLUP);
    pinMode(pinOpen, INPUT_PULLUP);
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
  Cover::StateE determineState(bool closedSensor, bool openSensor);
  bool isClosed(bool closedSensor, bool openSensor);
  bool isOpen(bool closedSensor, bool openSensor);
  bool isClosing(bool closedSensor, bool openSensor);
  bool isOpening(bool closedSensor, bool openSensor);

  uint8_t mPinClosed;
  uint8_t mPinOpen;
};
