#pragma once

#include <Stream.h>
#include <stdint.h>

#include "Util.h"

class IComponent {
 public:
  enum class Type { binarySensor, sensor, cover };

 protected:
  virtual ~IComponent() = default;

  virtual bool hasService() = 0;

  virtual void print(Stream& stream, uint8_t service) = 0;

  virtual void print(Stream& stream) = 0;

  virtual void setReported() = 0;

  virtual uint32_t timeSinceLastReport() const = 0;

  virtual uint8_t getEntityId() const = 0;

  virtual const char* getName() const = 0;
};

class Component : public virtual IComponent {
 public:
  explicit Component(uint8_t entityId) : mEntityId{entityId} {}

  Component(uint8_t entityId, const char* name) : mEntityId{entityId} {
    strncpy(mName, name, maxNameLength);
    mName[maxNameLength - 1] = '\0';
  }

  void setReported() override { mLastReportTime = seconds(); }

  uint32_t timeSinceLastReport() const final {
    return seconds() - mLastReportTime;
  }

  uint8_t getEntityId() const final { return mEntityId; }

  const char* getName() const final { return mName; }

 private:
  static const uint8_t maxNameLength = 20 + 1;  // 20 chars + null terminator

  uint32_t mLastReportTime = {};  // s
  const uint8_t mEntityId = {};
  char mName[maxNameLength] = {};
};
