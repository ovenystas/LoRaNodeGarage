/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>

#include "components/Cover.h"

class GarageCover: public Cover {
public:
  GarageCover(uint8_t entityId, const char* name,
      uint8_t pinClosed, uint8_t pinOpen, uint8_t pinRelay) :
      Cover(entityId, name),
      mPinClosed { pinClosed }, mPinOpen { pinOpen }, mPinRelay { pinRelay } {

    pinMode(pinClosed, INPUT_PULLUP);
    pinMode(pinOpen, INPUT_PULLUP);
    pinMode(pinRelay, OUTPUT);
  }

  bool update() override;

  void callService(const Cover::Service service) override;

  virtual DeviceClass getDeviceClass() const override {
    return DeviceClass::garage;
  }

private:
  Cover::State determineState();

  bool isClosed(bool closedSensor, bool openSensor);
  bool isOpen(bool closedSensor, bool openSensor);
  bool isClosing(bool closedSensor, bool openSensor);
  bool isOpening(bool closedSensor, bool openSensor);

  void activateRelay(uint8_t times);

  uint8_t mPinClosed;
  uint8_t mPinOpen;
  uint8_t mPinRelay;
};
