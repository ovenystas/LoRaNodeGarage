/*
 *  Created on: 21 apr. 2021
 *      Author: oveny
 */

// TODO: Load/save config items in EEPROM
#pragma once

#include <stdint.h>

#include "Unit.h"
#include "Util.h"

template <class T>
class ConfigItem {
 public:
  explicit ConfigItem(uint8_t configId, T value = 0,
                      Unit::Type unitType = Unit::Type::none,
                      uint8_t precision = 0)
      : mConfigId{configId},
        mValue{value},
        mUnit{unitType},
        mPrecision{precision > 3 ? static_cast<uint8_t>(3) : precision},
        mScaleFactor{factors[mPrecision]} {}

  inline uint8_t getPrecision() const { return mPrecision; }

  inline int16_t getScaleFactor() const { return mScaleFactor; }

  inline Unit getUnit() const { return mUnit; }

  inline T getValue() const { return mValue; }

  inline void setValue(T value = {}) { mValue = value; }

  inline uint8_t getConfigId() const { return mConfigId; }

  uint8_t writeDiscoveryItem(uint8_t* buffer) {
    buffer[0] = mConfigId;
    buffer[1] = static_cast<uint8_t>(mUnit.getType());
    buffer[2] = (sizeof(T) << 4) | mPrecision;
    return 3;
  }

  uint8_t writeConfigItemValue(uint8_t* buffer) {
    buffer[0] = mConfigId;
    T* p = reinterpret_cast<T*>(&buffer[1]);
    *p = hton(mValue);
    return 1 + sizeof(T);
  }

  uint8_t setConfigValue(uint8_t configId, const uint8_t* value) {
    if (configId == mConfigId) {
      mValue = ntoh(*(reinterpret_cast<const T*>(value)));
      return 1 + static_cast<uint8_t>(sizeof(T));
    }
    return 0;
  }

 private:
  int16_t factors[4] = {1, 10, 100, 1000};

  const uint8_t mConfigId;
  T mValue;
  const Unit mUnit;
  const uint8_t mPrecision;
  const int16_t mScaleFactor;
};
