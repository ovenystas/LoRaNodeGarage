#pragma once

#include "Component.h"
#include "Cover.h"
#include "Number.h"
#include "Util.h"

namespace GarageCoverConstants {
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
}  // namespace GarageCoverConstants

class GarageCover final : public IComponent {
 public:
  GarageCover() = delete;

  GarageCover(uint8_t entityId, const char* name, uint8_t pinClosed,
              uint8_t pinOpen, uint8_t pinRelay)
      : mCover{Cover(entityId, name, CoverDeviceClass::GARAGE)},

        mPinClosed{pinClosed},
        mPinOpen{pinOpen},
        mPinRelay{pinRelay} {
    pinMode(pinClosed, INPUT_PULLUP);
    pinMode(pinOpen, INPUT_PULLUP);
    pinMode(pinRelay, OUTPUT);
    digitalWrite(pinRelay, LOW);
  }

  void callService(uint8_t service) final;

  void loadConfigValues() final {};

  bool getDiscoveryEntity(DiscoveryEntityT& item) const final;

  uint8_t getEntityId() const final { return mCover.getEntityId(); }

  void getValueItem(ValueItemT& item) const final {
    return mCover.getValueItem(item);
  }

  bool setValueItem(const ValueItemT& item) final;

  bool isReportDue() const final { return mCover.isReportDue(); }

  size_t printTo(Print& p) const final { return mCover.printTo(p); };

  size_t printTo(Print& p, uint8_t service) const final {
    return mCover.printTo(p, service);
  };

  void setReported() final { mCover.setReported(); }

  bool update() final;

 private:
  CoverState determineState();

  bool isClosed(bool closedSensor, bool openSensor);
  bool isOpen(bool closedSensor, bool openSensor);
  bool isClosing(bool closedSensor, bool openSensor);
  bool isOpening(bool closedSensor, bool openSensor);

  void activateRelay(uint8_t times);

#ifdef PIO_UNIT_TESTING
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
