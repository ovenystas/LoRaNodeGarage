#pragma once

#include <stdint.h>

#include "Util.h"

class BaseComponent {
 public:
  enum class Type : uint8_t { binarySensor, sensor, cover };

  BaseComponent() = delete;

  explicit BaseComponent(uint8_t entityId) : mEntityId{entityId} {}

  BaseComponent(uint8_t entityId, const char* name)
      : mName{name}, mEntityId{entityId} {}

  bool isReportDue() const { return mIsReportDue; }

  void setReported() {
    mIsReportDue = false;
    mLastReportTime = millis();
  }

  void setIsReportDue(bool isDue) { mIsReportDue = isDue; }

  uint32_t timeSinceLastReport() const { return millis() - mLastReportTime; }

  uint8_t getEntityId() const { return mEntityId; }

  size_t printTo(Print& p) const {
    size_t n = 0;
    n += p.print(reinterpret_cast<const __FlashStringHelper*>(mName));
    return n;
  }

 private:
  uint32_t mLastReportTime{};  // ms
  const char* mName{""};
  const uint8_t mEntityId{};
  bool mIsReportDue{true};
};
