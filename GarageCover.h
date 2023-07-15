#pragma once

#include "Component.h"
#include "Cover.h"

class GarageCover : public IComponent {
 public:
  virtual ~GarageCover() = default;

  GarageCover() = delete;

  GarageCover(uint8_t entityId, const char* name, uint8_t pinClosed,
              uint8_t pinOpen, uint8_t pinRelay)
      : mCover{Cover(entityId, name, CoverDeviceClass::garage)},
        mPinClosed{pinClosed},
        mPinOpen{pinOpen},
        mPinRelay{pinRelay} {
    pinMode(pinClosed, INPUT_PULLUP);
    pinMode(pinOpen, INPUT_PULLUP);
    pinMode(pinRelay, OUTPUT);
    digitalWrite(pinRelay, LOW);
  }

  void callService(uint8_t service) final;

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) final {
    (void)buffer;
    return 0;
  }

  uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  uint8_t getEntityId() const final { return mCover.getEntityId(); }

  uint8_t getValueMsg(uint8_t* buffer) final {
    return mCover.getValueMsg(buffer);
  }

  size_t print(Stream& stream) final { return mCover.print(stream); };

  size_t print(Stream& stream, uint8_t service) final {
    return mCover.print(stream, service);
  };

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final {
    (void)numberOfConfigs;
    (void)buffer;
    return false;
  }

  void setReported() final { mCover.setReported(); }

  bool update() final;

 private:
  CoverState determineState();

  bool isClosed(bool closedSensor, bool openSensor);
  bool isOpen(bool closedSensor, bool openSensor);
  bool isClosing(bool closedSensor, bool openSensor);
  bool isOpening(bool closedSensor, bool openSensor);

  void activateRelay(uint8_t times);

#ifdef TESTING
 public:
#else
 private:
#endif
  Cover mCover;

 private:
  const uint8_t mPinClosed;
  const uint8_t mPinOpen;
  const uint8_t mPinRelay;
};
