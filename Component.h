#pragma once

#include <Stream.h>
#include <stdbool.h>
#include <stdint.h>

class IComponent {
 public:
  virtual void callService(uint8_t service) = 0;

  virtual uint8_t getConfigItemValuesMsg(uint8_t* buffer) = 0;

  virtual uint8_t getDiscoveryMsg(uint8_t* buffer) = 0;

  virtual uint8_t getEntityId() const = 0;

  virtual uint8_t getValueMsg(uint8_t* buffer) = 0;

  virtual void print(Stream& stream) = 0;

  virtual void print(Stream& stream, uint8_t service) = 0;

  virtual bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) = 0;

  virtual void setReported() = 0;

  virtual bool update() = 0;
};
