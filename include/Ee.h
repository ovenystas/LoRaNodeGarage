#pragma once

#include <CRC8.h>
#include <EEPROM.h>
#include <stdint.h>

#include "EeAdressMap.h"
#include "Number.h"

namespace Ee {

// EEPROM storage layout constants
static constexpr uint16_t VALUE_SIZE = sizeof(uint32_t);       // 4 bytes
static constexpr uint16_t CRC_SIZE = 1;                        // 1 byte
static constexpr uint16_t TOTAL_SIZE = VALUE_SIZE + CRC_SIZE;  // 5 bytes total

/**
 * @brief Helper template to get maximum value for a type without <limits>
 *
 * Used for truncation detection during type casting.
 * Specializations for common Arduino integer types.
 */
template <typename T>
inline constexpr uint32_t getMaxValue() {
  // Default implementation - works for most types
  return static_cast<uint32_t>(~static_cast<T>(0));
}

// Specializations for common types
template <>
inline constexpr uint32_t getMaxValue<uint8_t>() {
  return UINT8_MAX;
}
template <>
inline constexpr uint32_t getMaxValue<uint16_t>() {
  return UINT16_MAX;
}
template <>
inline constexpr uint32_t getMaxValue<int8_t>() {
  return INT8_MAX;
}
template <>
inline constexpr uint32_t getMaxValue<int16_t>() {
  return INT16_MAX;
}

/**
 * @brief Status codes for EEPROM load operations
 *
 * Provides detailed information about why an EEPROM load succeeded or failed.
 * Useful for debugging and logging in production systems.
 */
enum class LoadStatus : uint8_t {
  SUCCESS = 0,               // Value loaded successfully
  ADDRESS_OUT_OF_RANGE = 1,  // EEPROM address invalid or out of bounds
  CRC_FAILED = 2,            // CRC validation failed (data corrupted)
  CAST_TRUNCATED = 3         // Value would be truncated during type cast
};

/**
 * @brief Add value bytes to CRC8 checksum (template version)
 *
 * Supports any integer type from uint8_t to uint64_t.
 * Automatically handles size differences at compile time.
 */
template <typename T>
static void addValueToCrc(CRC8& crc, T value) {
  for (size_t i = 0; i < sizeof(T); i++) {
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
LoadStatus loadValue(uint16_t eeAddress, Number<T>& number, T defaultValue) {
  // Check EEPROM address validity
  if (eeAddress + TOTAL_SIZE > EEPROM.length()) {
    number.setValue(defaultValue);
    return LoadStatus::ADDRESS_OUT_OF_RANGE;
  }

  uint32_t storedValue = 0;
  if (load(eeAddress, storedValue)) {
    // Check for potential data loss due to type narrowing (for types smaller
    // than uint32_t) Only needed when sizeof(T) < sizeof(uint32_t)
    if (sizeof(T) < sizeof(uint32_t)) {
      if (storedValue > getMaxValue<T>()) {
        // Value would be truncated - use default instead
        number.setValue(defaultValue);
        (void)save(eeAddress, static_cast<uint32_t>(defaultValue));
        return LoadStatus::CAST_TRUNCATED;
      }
    }

    // Safe to cast and set value
    T typedValue = static_cast<T>(storedValue);
    number.setValue(typedValue);
    return LoadStatus::SUCCESS;
  } else {
    // EEPROM load failed (CRC mismatch), use default and save it
    number.setValue(defaultValue);
    (void)save(eeAddress, static_cast<uint32_t>(defaultValue));
    return LoadStatus::CRC_FAILED;
  }
}

}  // namespace Ee
