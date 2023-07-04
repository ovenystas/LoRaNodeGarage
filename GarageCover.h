#pragma once

#include <stdint.h>

#include "Cover.h"

class IGarageCover : public virtual ICover {
 public:
  virtual ~IGarageCover() = default;
};

class GarageCover : public virtual IGarageCover, public Cover {
 public:
  GarageCover(uint8_t entityId, const char* name, uint8_t pinClosed,
              uint8_t pinOpen, uint8_t pinRelay)
      : Cover(entityId, name, CoverDeviceClass::garage),
        mPinClosed{pinClosed},
        mPinOpen{pinOpen},
        mPinRelay{pinRelay} {
    pinMode(pinClosed, INPUT_PULLUP);
    pinMode(pinOpen, INPUT_PULLUP);
    pinMode(pinRelay, OUTPUT);
    digitalWrite(pinRelay, LOW);
  }

  bool update() final;

  void callService(uint8_t service) override;

  uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) final {
    (void)buffer;
    return 0;
  }

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final {
    (void)numberOfConfigs;
    (void)buffer;
    return false;
  }

 private:
  CoverState determineState();

  bool isClosed(bool closedSensor, bool openSensor);
  bool isOpen(bool closedSensor, bool openSensor);
  bool isClosing(bool closedSensor, bool openSensor);
  bool isOpening(bool closedSensor, bool openSensor);

  void activateRelay(uint8_t times);

  uint8_t mPinClosed;
  uint8_t mPinOpen;
  uint8_t mPinRelay;
};
