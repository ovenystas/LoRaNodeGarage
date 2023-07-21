/*
 *  Created on: 21 apr. 2021
 *      Author: oveny
 */

// TODO: Load/save config items in EEPROM
#pragma once

#include <stdint.h>

#include "Types.h"
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

  void getDiscoveryConfigItem(DiscoveryConfigItemT* item) const {
    item->configId = mConfigId;
    item->unit = static_cast<uint8_t>(mUnit.getType());
    item->size = sizeof(T);
    item->precision = mPrecision;
  }

  void getConfigItemValue(ConfigItemValueT* item) const {
    item->configId = mConfigId;
    item->value = static_cast<uint32_t>(mValue);
  }

  uint8_t setConfigItemValue(const ConfigItemValueT* item) {
    if (item->configId == mConfigId) {
      mValue = static_cast<const T>(item->value);
      return 1;
    }
    return 0;
  }

 private:
  const int16_t factors[4] = {1, 10, 100, 1000};  // TODO: Move out of class

  const uint8_t mConfigId;
  T mValue;
  const Unit mUnit;
  const uint8_t mPrecision;
  const int16_t mScaleFactor;
};
