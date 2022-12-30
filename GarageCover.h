/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>

#include "Cover.h"

class GarageCover : public Cover {
 public:
  GarageCover(uint8_t entityId, const char* name, uint8_t pinClosed,
              uint8_t pinOpen, uint8_t pinRelay)
      : Cover(entityId, name),
        mPinClosed{pinClosed},
        mPinOpen{pinOpen},
        mPinRelay{pinRelay} {
    pinMode(pinClosed, INPUT_PULLUP);
    pinMode(pinOpen, INPUT_PULLUP);
    pinMode(pinRelay, OUTPUT);
  }

  bool update() final;

  void callService(const Cover::Service service) override;

  inline DeviceClass getDeviceClass() const final {
    return DeviceClass::garage;
  }

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  virtual uint8_t getConfigItemValuesMsg(uint8_t* buffer) final {
    (void)buffer;
    return 0;
  }

  virtual void setConfigs(uint8_t numberOfConfigs,
                          const uint8_t* buffer) final {
    (void)numberOfConfigs;
    (void)buffer;
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
