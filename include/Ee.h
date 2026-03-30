#pragma once

#include <CRC8.h>
#include <EEPROM.h>

#include "EeAdressMap.h"
#include "Number.h"

namespace Ee {

static void addValueToCrc(CRC8& crc, uint32_t value) {
    for (uint8_t i = 0; i < sizeof(uint32_t); i++) {
        uint8_t b = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
        crc.add(b);
    }
}

inline bool load(uint16_t eeAddress, uint32_t &value) {
    if (eeAddress >= EEPROM.length()) {
      return false;
    }
    
    uint32_t localValue = 0;
    (void)EEPROM.get(eeAddress, localValue);
    uint8_t eeCrc = EEPROM.read(eeAddress + sizeof(uint32_t));

    CRC8 crc;
    addValueToCrc(crc, localValue);
    uint8_t valueCrc = crc.calc();

    if (eeCrc == valueCrc) {
      value = localValue;
      return true;
    }

    return false;
}

inline void save(uint16_t eeAddress, const uint32_t &value) {
    if (eeAddress >= EEPROM.length()) {
      return;
    }

    CRC8 crc;
    addValueToCrc(crc, value);
    uint8_t valueCrc = crc.calc();

    (void)EEPROM.put(eeAddress, value);
    EEPROM.put(eeAddress + sizeof(uint32_t), valueCrc);
}

template <typename T> void loadValue(uint16_t eeAddress, Number<T>& number, T defaultValue) {
    if (eeAddress >= EEPROM.length()) {
      number.setValue(defaultValue);
      return;
    }

    uint32_t storedValue = 0;
    if (load(eeAddress, storedValue)) {
      // Cast from uint32_t to the template type T
      T typedValue = static_cast<T>(storedValue);
      number.setValue(typedValue);
    } else {
      // EEPROM load failed or CRC mismatch, use default and save it
      number.setValue(defaultValue);
      save(eeAddress, static_cast<uint32_t>(defaultValue));
    }
}

}
