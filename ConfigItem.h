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
#include "ValueItem.h"

template <class T>
class ConfigItem {
 public:
  explicit ConfigItem(uint8_t configId, T value = 0,
                      Unit::Type unitType = Unit::Type::none,
                      uint8_t precision = 0)
      : mConfigId{configId},
        mValueItem{ValueItem<T>(unitType, precision, value)} {}

  uint8_t getPrecision() const { return mValueItem.getPrecision(); }

  int16_t getScaleFactor() const { return mValueItem.getScaleFactor(); }

  Unit getUnit() const { return mValueItem.getUnit(); }

  T getValue() const { return mValueItem.getValue(); }

  void setValue(T value = {}) { mValueItem.setValue(value); }

  uint8_t getConfigId() const { return mConfigId; }

  void getDiscoveryConfigItem(DiscoveryConfigItemT* item) const {
    item->configId = mConfigId;
    item->unit = static_cast<uint8_t>(mValueItem.getUnit().type());
    item->isSigned = mValueItem.isSigned();
    item->size = static_cast<uint8_t>(mValueItem.getValueSize()) / 2;
    item->precision = mValueItem.getPrecision();
  }

  void getConfigItemValue(ConfigItemValueT* item) const {
    item->configId = mConfigId;
    item->value = static_cast<uint32_t>(mValueItem.getValue());
  }

  uint8_t setConfigItemValue(const ConfigItemValueT* item) {
    if (item->configId == mConfigId) {
      mValueItem.setValue(static_cast<const T>(item->value));
      return 1;
    }
    return 0;
  }

 private:
  const uint8_t mConfigId;
  ValueItem<T> mValueItem;
};
