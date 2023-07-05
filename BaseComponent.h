#pragma once

#include <Stream.h>
#include <stdint.h>

#include "Util.h"

class IBaseComponent {
 public:
  enum class Type { binarySensor, sensor, cover };

  virtual void callService(uint8_t service) = 0;

  virtual bool hasService() = 0;

  virtual uint8_t getConfigItemValuesMsg(uint8_t* buffer) = 0;

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) = 0;

  virtual uint8_t getValueMsg(uint8_t* buffer) = 0;

  virtual void print(Stream& stream, uint8_t service) = 0;

  virtual void print(Stream& stream) = 0;

  virtual bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) = 0;

  virtual bool update() = 0;

 protected:
  virtual ~IBaseComponent() = default;

  virtual void setReported() = 0;

  virtual uint32_t timeSinceLastReport() const = 0;

  virtual uint8_t getEntityId() const = 0;

  virtual const char* getName() const = 0;
};

class BaseComponent : public virtual IBaseComponent {
 public:
  explicit BaseComponent(uint8_t entityId) : mEntityId{entityId} {}

  BaseComponent(uint8_t entityId, const char* name)
      : mEntityId{entityId}, mName{name} {}

  void setReported() override { mLastReportTime = seconds(); }

  uint32_t timeSinceLastReport() const final {
    return seconds() - mLastReportTime;
  }

  uint8_t getEntityId() const final { return mEntityId; }

  const char* getName() const final { return mName; }

 private:
  uint32_t mLastReportTime = {};  // s
  const uint8_t mEntityId = {};
  const char* mName = "";
};
