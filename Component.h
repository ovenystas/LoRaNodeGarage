/*
 *  Created on: 21 mars 2021
 *      Author: oveny
 */

#pragma once

#include <Stream.h>
#include <stdint.h>

#include "ConfigItem.h"

class Component {
 public:
  enum class Type { binarySensor, sensor, cover };

  Component(uint8_t entityId) : mEntityId{entityId} {}

  Component(uint8_t entityId, const char* name) : mEntityId{entityId} {
    strncpy(mName, name, maxNameLength);
    mName[maxNameLength - 1] = '\0';
  }

  virtual ~Component() = default;

  virtual bool update() = 0;

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) = 0;

  virtual uint8_t getConfigItemValuesMsg(uint8_t* buffer) = 0;

  virtual uint8_t getValueMsg(uint8_t* buffer) = 0;

  virtual bool hasService() = 0;

  virtual void callService(uint8_t service) = 0;

  virtual void setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) = 0;

  virtual void print(Stream& stream, uint8_t service) = 0;

  virtual void print(Stream& stream) = 0;

  virtual void setReported() { mLastReportTime = seconds(); }

  uint32_t timeSinceLastReport() const { return seconds() - mLastReportTime; }

  inline uint8_t getEntityId() const { return mEntityId; }

  inline const char* getName() const { return mName; }

 protected:
  uint32_t mLastReportTime = {};  // s

 private:
  static const uint8_t maxNameLength = 20 + 1;  // 20 chars + null terminator

  const uint8_t mEntityId = {};
  char mName[maxNameLength] = {};
};
