#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "Util.h"

class BaseComponent {
 public:
  enum class Type : uint8_t {
    BINARY_SENSOR,
    BUTTON,
    CAMERA,
    CLIMATE,
    COVER,
    DEVICE_TRACKER,
    FAN,
    HUMIDIFIER,
    IMAGE,
    LAWN_MOWER,
    LIGHT,
    LOCK,
    NOTIFY,
    NUMBER,
    ROOM_PRESENCE,
    SELECT,
    SENSOR,
    SIREN,
    SWITCH,
    TEXT,
    VACUUM,
    VALVE,
    WATER_HEATER
  };

  enum class Category : uint8_t { NONE, CONFIG, DIAGNOSTIC };

  BaseComponent() = delete;

  explicit BaseComponent(uint8_t entityId) : mEntityId{entityId} {}

  BaseComponent(uint8_t entityId, const char* name)
      : mName{name}, mEntityId{entityId} {}

  BaseComponent(uint8_t entityId, const char* name, Category category)
      : mName{name}, mEntityId{entityId}, mCategory{category} {}

  BaseComponent(uint8_t entityId, Category category)
      : mEntityId{entityId}, mCategory{category} {}

  bool isReportDue() const { return mIsReportDue; }

  void setReported() {
    mIsReportDue = false;
    mLastReportTime = millis();
  }

  void setIsReportDue(bool isDue) { mIsReportDue = isDue; }

  uint32_t timeSinceLastReport() const { return millis() - mLastReportTime; }

  uint8_t getEntityId() const { return mEntityId; }

  Category getCategory() const { return mCategory; }

  const char* getName() const { return mName; }

  size_t printTo(Print& p) const {
    size_t n = 0;
    n += p.print(reinterpret_cast<const __FlashStringHelper*>(mName));
    return n;
  }

 private:
  uint32_t mLastReportTime{};  // ms
  const char* mName{nullptr};
  const uint8_t mEntityId{};
  const Category mCategory{Category::NONE};
  bool mIsReportDue{true};
};
