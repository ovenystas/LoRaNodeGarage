#pragma once

#include "Util.h"

#define IS_SIGNED_TYPE(type) (type(-1) < type(0))
#define CONFIGVALUES_MAX 8

struct DiscoveryEntityItemT {
  uint8_t entityId;
  uint8_t componentType;
  uint8_t deviceClass;
  uint8_t unit;
  uint8_t precision : 2;
  uint8_t sizeCode : 2;
  uint8_t isSigned : 1;
  uint8_t reserved : 3;

  size_t size() const { return 5; }

  bool operator==(const DiscoveryEntityItemT& rhs) const {
    return entityId == rhs.entityId && componentType == rhs.componentType &&
           deviceClass == rhs.deviceClass && unit == rhs.unit &&
           isSigned == rhs.isSigned && sizeCode == rhs.sizeCode &&
           precision == rhs.precision;
  }

  bool operator!=(const DiscoveryEntityItemT& rhs) const {
    return !(*this == rhs);
  }

  size_t toByteArray(uint8_t* buf, size_t length) const {
    if (length < size()) {
      return 0;
    }
    buf[0] = entityId;
    buf[1] = componentType;
    buf[2] = deviceClass;
    buf[3] = unit;
    buf[4] = (isSigned << 4) | (sizeCode << 2) | precision;
    return size();
  }
};

struct DiscoveryConfigItemT {
  uint8_t configId;
  uint8_t unit;
  uint8_t precision : 2;
  uint8_t sizeCode : 2;
  uint8_t isSigned : 1;
  uint8_t reserved : 3;

  size_t size() const { return 3; }

  bool operator==(const DiscoveryConfigItemT& rhs) const {
    return configId == rhs.configId && unit == rhs.unit &&
           isSigned == rhs.isSigned && sizeCode == rhs.sizeCode &&
           precision == rhs.precision;
  }

  bool operator!=(const DiscoveryConfigItemT& rhs) const {
    return !(*this == rhs);
  }

  size_t toByteArray(uint8_t* buf, size_t length) const {
    if (length < sizeof(*this)) {
      return 0;
    }

    buf[0] = configId;
    buf[1] = unit;
    buf[2] = (isSigned << 4) | (sizeCode << 2) | precision;

    return size();
  }
};

// TODO: See if the array size can be templatized
// // N is number of config items it can hold.
// template <size_t N>
struct DiscoveryItemT {
  DiscoveryEntityItemT entity;
  uint8_t numberOfConfigItems;
  DiscoveryConfigItemT configItems[13];

  size_t size() const {
    return entity.size() + sizeof(numberOfConfigItems) +
           configItems[0].size() * numberOfConfigItems;
  }

  bool operator==(const DiscoveryItemT& rhs) const {
    if (entity != rhs.entity) {
      return false;
    }
    if (numberOfConfigItems != rhs.numberOfConfigItems) {
      return false;
    }
    if (numberOfConfigItems > 13) {
      return false;
    }
    for (size_t i = 0; i < numberOfConfigItems; i++) {
      if (configItems[i] != rhs.configItems[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const DiscoveryItemT& rhs) const { return !(*this == rhs); }

  size_t toByteArray(uint8_t* buf, size_t length) const {
    if (length < entity.size() + sizeof(numberOfConfigItems) +
                     numberOfConfigItems * configItems[0].size()) {
      return 0;
    }

    size_t n = 0;
    n += entity.toByteArray(&buf[n], length - n);
    buf[n++] = numberOfConfigItems;
    for (uint8_t i = 0; i < numberOfConfigItems; i++) {
      n += configItems[i].toByteArray(&buf[n], length - n);
    }

    return n;
  }
};

struct ValueItemT {
  uint8_t entityId;
  uint32_t value;

  static constexpr size_t size() { return 5; }

  ValueItemT(uint8_t _enitityId = 0, uint32_t _value = 0)
      : entityId{_enitityId}, value{_value} {}

  bool operator==(const ValueItemT& rhs) const {
    return entityId == rhs.entityId && value == rhs.value;
  }

  bool operator!=(const ValueItemT& rhs) const { return !(*this == rhs); }

  size_t toByteArray(uint8_t* buf, size_t length) const {
    if (length < size()) {
      return 0;
    }

    buf[0] = entityId;
    uint32_t* pValue_n = reinterpret_cast<uint32_t*>(&buf[1]);
    *pValue_n = hton(value);

    return size();
  }

  size_t fromByteArray(const uint8_t* buf, size_t length) {
    if (length < size()) {
      return 0;
    }

    entityId = buf[0];
    const uint32_t* pValue_n = reinterpret_cast<const uint32_t*>(&buf[1]);
    value = ntoh(*pValue_n);
    return size();
  }
};

struct ConfigItemValueT {
  uint8_t configId;
  uint32_t value;

  size_t size() const { return sizeof(configId) + sizeof(value); }

  bool operator==(const ConfigItemValueT& rhs) const {
    return configId == rhs.configId && value == rhs.value;
  }

  bool operator!=(const ConfigItemValueT& rhs) const { return !(*this == rhs); }

  size_t toByteArray(uint8_t* buf, size_t length) const {
    if (length < size()) {
      return 0;
    }
    buf[0] = configId;
    uint32_t* pValue_n = reinterpret_cast<uint32_t*>(&buf[1]);
    *pValue_n = hton(value);
    return size();
  }
};

struct ConfigValuePayloadT {
  uint8_t entityId;
  uint8_t numberOfConfigs;
  ConfigItemValueT configValues[CONFIGVALUES_MAX];

  size_t size() const {
    return sizeof(entityId) + sizeof(numberOfConfigs) +
           configValues[0].size() * CONFIGVALUES_MAX;
  }

  size_t toByteArray(uint8_t* buf, size_t length) const {
    if (length < size()) {
      return 0;
    }

    buf[0] = entityId;
    buf[1] = numberOfConfigs;

    size_t n = 2;
    for (uint8_t i = 0; i < numberOfConfigs; i++) {
      n += configValues[i].toByteArray(&buf[n], length - n);
    }

    return n;
  }

  uint8_t fromByteArray(const uint8_t* buf, size_t length) {
    if (length != 2 + buf[1] * configValues[0].size()) {
      return 0;
    }

    entityId = buf[0];
    numberOfConfigs = buf[1];

    size_t idx = 2;
    for (uint8_t i = 0; i < numberOfConfigs; i++) {
      configValues[i].configId = buf[idx++];
      const uint32_t* pValue_n = reinterpret_cast<const uint32_t*>(&buf[idx]);
      configValues[i].value = ntoh(*pValue_n);
      idx += 4;
    }
    return 1;
  }
};
