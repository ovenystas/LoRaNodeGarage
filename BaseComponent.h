#pragma once

#include <stdint.h>

#include "Util.h"

class BaseComponent {
 public:
  enum class Type { binarySensor, sensor, cover };

  explicit BaseComponent(uint8_t entityId) : mEntityId{entityId} {}

  BaseComponent(uint8_t entityId, const char* name)
      : mEntityId{entityId}, mName{name} {}

  void setReported() { mLastReportTime = seconds(); }

  uint32_t timeSinceLastReport() const { return seconds() - mLastReportTime; }

  uint8_t getEntityId() const { return mEntityId; }

  const char* getName() const { return mName; }

 private:
  uint32_t mLastReportTime = {};  // s
  const uint8_t mEntityId = {};
  const char* mName = "";
};
