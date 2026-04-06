#pragma once

#include "Ee.h"
#include "Number.h"
#include "Types.h"

/**
 * @brief Decorator class that adds EEPROM persistence to Number<T>
 *
 * This class wraps a Number<T> instance and adds automatic saving to EEPROM
 * when values are set, plus manual loading from EEPROM during setup.
 *
 * Usage:
 *   // Create a persistent number with EEPROM address
 *   PersistentNumber<uint16_t> myNumber(0x10, 5, "My Value",
 * NumberDeviceClass::NONE, Unit::Type::none, 0, 0, 1000, 500);
 *
 *   // In setup(): load value from EEPROM or use default
 *   myNumber.loadFromEeprom(500);  // default value if not in EEPROM
 *
 *   // In main loop: setValue() automatically saves to EEPROM
 *   myNumber.setValue(750);
 */
template <class T>
class PersistentNumber {
 public:
  // Compile-time validation: T must fit in uint32_t
  static_assert(sizeof(T) <= sizeof(uint32_t),
                "PersistentNumber<T>: T must fit in uint32_t");

  /**
   * @brief Construct persistent number with full parameters
   * @param eeAddress EEPROM address where value will be stored
   * @param entityId Unique entity identifier
   * @param name Human-readable name
   * @param deviceClass Number device class (optional)
   * @param unitType Unit type (optional)
   * @param precision Decimal precision (0-3)
   * @param category Component category (optional)
   * @param value Initial value
   * @param min_value Minimum allowed value
   * @param max_value Maximum allowed value
   */
  PersistentNumber(
      uint16_t eeAddress, uint8_t entityId, const char* name,
      NumberDeviceClass deviceClass = NumberDeviceClass::NONE,
      Unit::Type unitType = Unit::Type::none, uint8_t precision = 0,
      BaseComponent::Category category = BaseComponent::Category::NONE,
      T value = T{}, T min_value = T{}, T max_value = T{})
      : mEeAddress{eeAddress},
        mNumber{Number<T>(entityId, name, deviceClass, unitType, precision,
                          category, value, min_value, max_value)} {}

  /**
   * @brief Construct persistent number with minimal parameters
   * @param eeAddress EEPROM address where value will be stored
   * @param entityId Unique entity identifier
   */
  PersistentNumber(uint16_t eeAddress, uint8_t entityId)
      : mEeAddress{eeAddress}, mNumber{Number<T>(entityId)} {}

  // ========== EEPROM Operations ==========

  /**
   * @brief Load value from EEPROM during setup
   * @param defaultValue Value to use if EEPROM load fails or CRC mismatch
   * @return LoadStatus indicating success or reason for failure
   *
   * Call this once in setup() to restore persisted values.
   * If the value in EEPROM is invalid (CRC mismatch), uses defaultValue.
   * Returns status information useful for debugging.
   */
  Ee::LoadStatus loadFromEeprom(T defaultValue) {
    mLastLoadStatus = Ee::loadValue(mEeAddress, mNumber, defaultValue);

    // Log errors if they occurred
    if (mLastLoadStatus != Ee::LoadStatus::SUCCESS) {
      logLoadError();
    }

    return mLastLoadStatus;
  }

  /**
   * @brief Get the status of the last EEPROM load operation
   * @return LoadStatus from most recent loadFromEeprom() call
   */
  Ee::LoadStatus getLastLoadStatus() const { return mLastLoadStatus; }

  /**
   * @brief Check if last load was successful
   * @return true if last load succeeded, false otherwise
   */
  bool wasLastLoadSuccessful() const {
    return mLastLoadStatus == Ee::LoadStatus::SUCCESS;
  }

  /**
   * @brief Manually save value to EEPROM
   *
   * This is called automatically whenever setValue() is used.
   * Can be called explicitly if needed.
   */
  void saveToEeprom() const {
    bool saved =
        Ee::save(mEeAddress, static_cast<uint32_t>(mNumber.getValue()));
    if (!saved) {
#ifdef ARDUINO
      Serial.print(F("[WARN] EEPROM save failed for entity "));
      Serial.print(mNumber.getEntityId());
      Serial.print(F(" at address 0x"));
      Serial.println(mEeAddress, HEX);
#endif
    }
  }

  // ========== Delegated Number Methods ==========

  BaseComponent::Type getComponentType() const {
    return mNumber.getComponentType();
  }

  NumberDeviceClass getDeviceClass() const { return mNumber.getDeviceClass(); }

  void getDiscoveryEntity(DiscoveryEntityT& item) const {
    mNumber.getDiscoveryEntity(item);
  }

  uint8_t getEntityId() const { return mNumber.getEntityId(); }

  Unit::Type getUnitType() const { return mNumber.getUnitType(); }

  T getValue() const { return mNumber.getValue(); }

  void getValueItem(ValueItemT& item) const { mNumber.getValueItem(item); }

  void setValueItem(const ValueItemT& item) {
    mNumber.setValueItem(item);
    saveToEeprom();
  }

  bool isReportDue() const { return mNumber.isReportDue(); }

  size_t printTo(Print& p) const { return mNumber.printTo(p); }

  void setIsReportDue(bool isDue) { mNumber.setIsReportDue(isDue); }

  void setReported() { mNumber.setReported(); }

  /**
   * @brief Set value and automatically save to EEPROM
   * @param value New value (will be clamped to min/max)
   */
  void setValue(T value) {
    mNumber.setValue(value);
    saveToEeprom();
  }

  uint32_t timeSinceLastReport() const { return mNumber.timeSinceLastReport(); }

  // ========== Direct Access (if needed) ==========

  /**
   * @brief Get reference to underlying Number for advanced access
   */
  Number<T>& getNumber() { return mNumber; }

  const Number<T>& getNumber() const { return mNumber; }

 private:
  Ee::LoadStatus mLastLoadStatus = Ee::LoadStatus::SUCCESS;
  uint16_t mEeAddress;
  Number<T> mNumber;

  /**
   * @brief Log EEPROM load error to Serial
   *
   * Provides diagnostic information about EEPROM failures.
   * This helps identify configuration corruption or EEPROM issues in the field.
   */
  void logLoadError() const {
    // Only log if Serial is available (Arduino environment)
#ifdef ARDUINO
    Serial.print(F("[EEPROM] Load failed for entity "));
    Serial.print(mNumber.getEntityId());
    Serial.print(F(" at 0x"));
    Serial.print(mEeAddress, HEX);
    Serial.print(F(" - Reason: "));

    switch (mLastLoadStatus) {
      case Ee::LoadStatus::ADDRESS_OUT_OF_RANGE:
        Serial.println(F("Address out of range"));
        break;
      case Ee::LoadStatus::CRC_FAILED:
        Serial.println(F("CRC validation failed (data corrupted)"));
        break;
      case Ee::LoadStatus::CAST_TRUNCATED:
        Serial.println(F("Value would be truncated during cast"));
        break;
      case Ee::LoadStatus::SUCCESS:
        // Should not reach here, but handle it anyway
        Serial.println(F("Success"));
        break;
      default:
        Serial.println(F("Unknown error"));
        break;
    }
#endif
  }
};

// Prevent instantiation with floating-point types (incomplete forward
// declarations)
template <>
class PersistentNumber<float>;
template <>
class PersistentNumber<double>;
template <>
class PersistentNumber<long double>;
