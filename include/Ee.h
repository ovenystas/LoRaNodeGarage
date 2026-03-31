#pragma once

#include <CRC8.h>
#include <EEPROM.h>

#include "EeAdressMap.h"
#include "Number.h"

namespace Ee {

// EEPROM storage layout constants
static constexpr uint16_t VALUE_SIZE = sizeof(uint32_t);       // 4 bytes
static constexpr uint16_t CRC_SIZE = 1;                        // 1 byte
static constexpr uint16_t TOTAL_SIZE = VALUE_SIZE + CRC_SIZE;  // 5 bytes total

static void addValueToCrc(CRC8& crc, uint32_t value) {
  for (uint8_t i = 0; i < sizeof(uint32_t); i++) {
    uint8_t b = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
    crc.add(b);
  }
}

inline bool load(uint16_t eeAddress, uint32_t& value) {
  // Check that we have enough space for value + CRC (5 bytes total)
  if (eeAddress + TOTAL_SIZE > EEPROM.length()) {
    return false;
  }

  uint32_t localValue = 0;
  (void)EEPROM.get(eeAddress, localValue);
  uint8_t eeCrc = EEPROM.read(eeAddress + VALUE_SIZE);

  CRC8 crc;
  addValueToCrc(crc, localValue);
  uint8_t valueCrc = crc.calc();

  if (eeCrc == valueCrc) {
    value = localValue;
    return true;
  }

  return false;
}

inline bool save(uint16_t eeAddress, const uint32_t& value) {
  // Check that we have enough space for value + CRC (5 bytes total)
  if (eeAddress + TOTAL_SIZE > EEPROM.length()) {
    return false;
  }

  CRC8 crc;
  addValueToCrc(crc, value);
  uint8_t valueCrc = crc.calc();

  (void)EEPROM.put(eeAddress, value);
  EEPROM.put(eeAddress + VALUE_SIZE, valueCrc);
  return true;
}

template <typename T>
void loadValue(uint16_t eeAddress, Number<T>& number, T defaultValue) {
  if (eeAddress + TOTAL_SIZE > EEPROM.length()) {
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
    (void)save(eeAddress, static_cast<uint32_t>(defaultValue));
  }
}

}  // namespace Ee
