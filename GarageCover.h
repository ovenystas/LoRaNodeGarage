#pragma once

#include "Component.h"
#include "ConfigItem.h"
#include "Cover.h"
#include "EeAdressMap.h"

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

  uint8_t getConfigItemValues(ConfigItemValueT* items,
                              uint8_t length) const final;

  void getDiscoveryItem(DiscoveryItemT* item) const final;

  uint8_t getEntityId() const final { return mCover.getEntityId(); }

  void getValueItem(ValueItemT* item) const final {
    return mCover.getValueItem(item);
  }

  bool isReportDue() const final { return mCover.isReportDue(); }

  size_t print(Stream& stream) const final { return mCover.print(stream); };

  size_t print(Stream& stream, uint8_t service) const final {
    return mCover.print(stream, service);
  };

  bool setConfigItemValues(const ConfigItemValueT* items, uint8_t length) final;

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
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {1};

    ConfigItem<uint16_t> reportInterval = {ConfigItem<uint16_t>(
        0, EE_ADDRESS_CONFIG_GARAGECOVER_0, 300, Unit::Type::s)};
  };

  Config mConfig;
  const uint8_t mPinClosed;
  const uint8_t mPinOpen;
  const uint8_t mPinRelay;
};
