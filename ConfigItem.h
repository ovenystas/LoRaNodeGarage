/*
 *  Created on: 21 apr. 2021
 *      Author: oveny
 */

// TODO: Load/save config items in EEPROM
#pragma once

#include <CRC8.h>
#include <EEPROM.h>
#include <stdint.h>

#include "Types.h"
#include "Unit.h"
#include "Util.h"
#include "ValueItem.h"

template <class T>
class ConfigItem {
 public:
  explicit ConfigItem(uint8_t configId, uint16_t eeAddress, T defaultValue = 0,
                      Unit::Type unitType = Unit::Type::none,
                      uint8_t precision = 0)
      : mConfigId{configId},
        mValueItem{ValueItem<T>(unitType, precision, defaultValue)},
        mDefaultValue{defaultValue},
        mEeAddress{eeAddress} {}

  uint8_t getPrecision() const { return mValueItem.getPrecision(); }

  int16_t getScaleFactor() const { return mValueItem.getScaleFactor(); }

  Unit getUnit() const { return mValueItem.getUnit(); }

  T getValue() const { return mValueItem.getValue(); }

  void setValue(T value = {}) {
    mValueItem.setValue(value);
    save();
  }

  uint8_t getConfigId() const { return mConfigId; }

  void getDiscoveryConfigItem(DiscoveryConfigItemT* item) const {
    item->configId = mConfigId;
    item->unit = static_cast<uint8_t>(mValueItem.getUnit().type());
    item->isSigned = mValueItem.isSigned();
    item->sizeCode = static_cast<uint8_t>(mValueItem.getValueSize()) / 2;
    item->precision = mValueItem.getPrecision();
  }

  void getConfigItemValue(ConfigItemValueT* item) const {
    item->configId = mConfigId;
    item->value = static_cast<uint32_t>(mValueItem.getValue());
  }

  uint8_t setConfigItemValue(const ConfigItemValueT* item) {
    if (item->configId == mConfigId) {
      mValueItem.setValue(static_cast<const T>(item->value));
      save();
      return 1;
    }
    return 0;
  }

  void load() {
    if (mEeAddress >= EEPROM.length()) {
      return;
    }

    T value;
    (void)EEPROM.get(mEeAddress, value);
    uint8_t eeCrc = EEPROM.read(mEeAddress + sizeof(T));

    CRC8 crc;
    addValueToCrc(crc, value);
    uint8_t valueCrc = crc.calc();

    if (eeCrc == valueCrc) {
      mValueItem.setValue(value);
    } else {
      mValueItem.setValue(mDefaultValue);
      save();
    }
  }

 private:
  void addValueToCrc(CRC8& crc, T value) {
    for (int8_t i = sizeof(T) - 1; i >= 0; i--) {
      uint8_t b = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
      crc.add(b);
    }
  }

  void save() {
    if (mEeAddress >= EEPROM.length()) {
      return;
    }

    T value = mValueItem.getValue();

    CRC8 crc;
    addValueToCrc(crc, value);
    uint8_t valueCrc = crc.calc();

    (void)EEPROM.put(mEeAddress, value);
    EEPROM.put(mEeAddress + sizeof(T), valueCrc);
  }

  const uint8_t mConfigId;
  ValueItem<T> mValueItem;
  T mDefaultValue;
  const uint16_t mEeAddress;
};
