#pragma once

#include <Arduino.h>

#include "Util.h"

#define IS_SIGNED_TYPE(type) (type(-1) < type(0))

static const uint8_t CONFIGVALUES_MAX = 8;

struct DiscoveryEntityT {
  uint8_t entityId;
  uint8_t componentType;
  uint8_t deviceClass;
  uint8_t category;
  uint8_t unit;
  uint8_t precision : 2;
  uint8_t sizeCode : 2;
  uint8_t isSigned : 1;
  uint8_t reserved : 3;
  uint32_t minValue;  // Converted to uint32_t for easier transmission, should
                      // be interpreted as signed if isSigned is true
  uint32_t maxValue;  // Converted to uint32_t for easier transmission, should
                      // be interpreted as signed if isSigned is true
  const char* name;   // Null-terminated string

  size_t size() const { return 14 + strlen_P(name) + 1; }

  size_t toByteArray(uint8_t* buf, size_t length) const {
    size_t actualSize = size();

    if (length < actualSize) {
      return 0;
    }

    buf[0] = entityId;
    buf[1] = componentType;
    buf[2] = deviceClass;
    buf[3] = category;
    buf[4] = unit;
    buf[5] = (isSigned << 4) | (sizeCode << 2) | precision;
    *(reinterpret_cast<uint32_t*>(&buf[6])) = hton(minValue);
    *(reinterpret_cast<uint32_t*>(&buf[10])) = hton(maxValue);
    strcpy_P((char*)&buf[14], name);

    return actualSize;
  }
};

struct ValueItemT {
  uint8_t entityId;
  uint32_t value;

  static constexpr size_t size() { return sizeof(entityId) + sizeof(value); }

  ValueItemT(uint8_t _entityId = 0, uint32_t _value = 0)
      : entityId{_entityId}, value{_value} {}

  bool operator==(const ValueItemT& rhs) const {
    return entityId == rhs.entityId && value == rhs.value;
  }

  bool operator!=(const ValueItemT& rhs) const { return !(*this == rhs); }

  size_t toByteArray(uint8_t* buf, size_t length) const {
    if (length < size()) {
      return 0;
    }

    buf[0] = entityId;
    *(reinterpret_cast<uint32_t*>(&buf[1])) = hton(value);

    return size();
  }

  size_t fromByteArray(const uint8_t* buf, size_t length) {
    if (length < size()) {
      return 0;
    }

    entityId = buf[0];
    value = ntoh(*(reinterpret_cast<const uint32_t*>(&buf[1])));

    return size();
  }
};
