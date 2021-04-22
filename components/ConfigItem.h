/*
 *  Created on: 21 apr. 2021
 *      Author: oveny
 */

// TODO: Load/save config items in EEPROM
#pragma once

#include <stdint.h>

#include "Unit.h"

template<class T>
class ConfigItem {
public:
  ConfigItem(uint8_t configId,
      T value = 0,
      Unit::Type unitType = Unit::Type::none,
      uint8_t precision = 0) :
        mConfigId { configId },
        mValue { value },
        mUnit { unitType },
        mPrecision { precision > 3 ? static_cast<uint8_t>(3) : precision },
        mScaleFactor { factors[mPrecision] } {
  }

  inline uint8_t getPrecision() const {
    return mPrecision;
  }

  inline int16_t getScaleFactor() const {
    return mScaleFactor;
  }

  inline Unit getUnit() const {
    return mUnit;
  }

  inline T getValue() const {
    return mValue;
  }

  inline void setValue(T value = { }) {
    mValue = value;
  }

  inline uint8_t getConfigId() const {
    return mConfigId;
  }

  uint8_t writeDiscoveryItem(uint8_t* buffer) {
    buffer[0] = mConfigId;
    buffer[1] = static_cast<uint8_t>(mUnit.getType());
    buffer[2] = (sizeof(T) << 4) | mPrecision;
    return 3;
  }

private:
  static constexpr int16_t factors[4] = { 1, 10, 100, 1000 };

  const uint8_t mConfigId;
  T mValue;
  const Unit mUnit;
  const uint8_t mPrecision;
  const int16_t mScaleFactor;
};
