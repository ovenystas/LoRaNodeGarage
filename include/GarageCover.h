#pragma once

#include "Component.h"
#include "Cover.h"
#include "Number.h"
#include "Util.h"

namespace GarageCoverConstants {
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;

static const char reportIntervalName[] PROGMEM = "Report Interval";
}  // namespace GarageCoverConstants


class GarageCover final : public IComponent {
 public:
  GarageCover() = delete;

  GarageCover(uint8_t entityId, const char* name, uint8_t pinClosed,
              uint8_t pinOpen, uint8_t pinRelay)
      : mCover{Cover(entityId, name, CoverDeviceClass::GARAGE)},

        mReportInterval{Number<uint16_t>(
        entityId + 1, GarageCoverConstants::reportIntervalName, NumberDeviceClass::DURATION, Unit::Type::s, 0,
        BaseComponent::Category::CONFIG, GarageCoverConstants::CONFIG_REPORT_INTERVAL_DEFAULT, 0,
        Util::TWELVE_HOURS_IN_SECONDS)},

        mPinClosed{pinClosed},
        mPinOpen{pinOpen},
        mPinRelay{pinRelay} {
          pinMode(pinClosed, INPUT_PULLUP);
          pinMode(pinOpen, INPUT_PULLUP);
          pinMode(pinRelay, OUTPUT);
          digitalWrite(pinRelay, LOW);
    }

  void callService(uint8_t service) final;

  uint8_t getConfigValueItems(ValueItemT* items, uint8_t length) const final;

    void loadConfigValues() final;

  uint8_t getDiscoveryItems(DiscoveryEntityItemT* item, uint8_t length) const final;

  uint8_t getEntityId() const final { return mCover.getEntityId(); }

  uint8_t getNumEntities() const final { return sNumItems; };

  void getValueItem(ValueItemT* item) const final {
    return mCover.getValueItem(item);
  }

  bool setValueItem(const ValueItemT &item) final;
  
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

  static constexpr uint8_t sNumConfigItems = 1;
  static constexpr uint8_t sNumItems = 1 + sNumConfigItems;

  #ifdef PIO_UNIT_TESTING 
  public:
  #else
  private:
  #endif
  Cover mCover;
  
 private:
  Number<uint16_t> mReportInterval;
  const uint8_t mPinClosed;
  const uint8_t mPinOpen;
  const uint8_t mPinRelay;
};
