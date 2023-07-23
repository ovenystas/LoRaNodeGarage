#pragma once

#include <stdint.h>

#include "Util.h"

class BaseComponent {
 public:
  enum class Type { binarySensor, sensor, cover };

  BaseComponent() = delete;

  explicit BaseComponent(uint8_t entityId) : mEntityId{entityId} {}

  BaseComponent(uint8_t entityId, const char* name)
      : mEntityId{entityId}, mName{name} {}

  bool isReportDue() const { return mIsReportDue; }

  void setReported() {
    mIsReportDue = false;
    mLastReportTime = seconds();
  }

  void setIsReportDue(bool isDue) { mIsReportDue = isDue; }

  uint32_t timeSinceLastReport() const { return seconds() - mLastReportTime; }

  uint8_t getEntityId() const { return mEntityId; }

  const char* getName() const { return mName; }

 private:
  bool mIsReportDue{true};
  uint32_t mLastReportTime{};  // s
  const uint8_t mEntityId{};
  const char* mName{""};
};
