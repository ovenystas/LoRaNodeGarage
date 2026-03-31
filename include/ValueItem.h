#pragma once

#include <Arduino.h>
#include <Printable.h>
#include <assert.h>
#include <stdint.h>

#include "Unit.h"

#define IS_SIGNED_TYPE(type) (type(-1) < type(0))

#define SIGNED_MIN(type) (static_cast<type>(1 << (sizeof(type) * 8 - 1)))
#define UNSIGNED_MIN(type) (0)

#define SIGNED_MAX(type) (~SIGNED_MIN(type))
#define UNSIGNED_MAX(type) (static_cast<type>(-1))

#define MIN_OF(type) \
  (IS_SIGNED_TYPE(type) ? SIGNED_MIN(type) : UNSIGNED_MIN(type))
#define MAX_OF(type) \
  (IS_SIGNED_TYPE(type) ? SIGNED_MAX(type) : UNSIGNED_MAX(type))

namespace ValueItemConstants {
static const uint16_t factors[4] = {1, 10, 100, 1000};
}

/**
 * @brief A class of value item which has value, unit, precision, min and max.
 * @tparam T of type int8_t, uint8_t, int16_t, uint16_t, int32_t or uint32_t
 */
template <class T>
class ValueItem : public Printable {
 public:
  ValueItem(Unit::Type unitType = Unit::Type::none, uint8_t precision = 0,
            T value = 0, T min_value = MIN_OF(T), T max_value = MAX_OF(T))
      : mValue{value},
        mMin{min_value},
        mMax{max_value},
        mUnit{Unit(unitType)},
        mPrecision{precision > 3 ? static_cast<uint8_t>(3) : precision} {
    assert(mValue >= mMin);
    assert(mValue <= mMax);
  }

  T getValue() const { return mValue; }

  void setValue(T value = {}) {
    if (value < mMin) {
      mValue = mMin;
    }

    else if (value > mMax) {
      mValue = mMax;
    }

    else {
      mValue = value;
    }
  }

  static constexpr bool isSigned() { return IS_SIGNED_TYPE(T); }

  /**
   * @brief Helper: Check if value is negative
   *
   * Works safely for both signed and unsigned types.
   * For unsigned types, always returns false.
   * Uses IS_SIGNED_TYPE macro for compile-time optimization.
   */
  bool isNegative() const {
    // Macro-based check: returns false for unsigned types at compile time
    return IS_SIGNED_TYPE(T) && mValue < 0;
  }

  /**
   * @brief Helper: Get absolute value
   *
   * For negative values, returns negated value.
   * For positive/unsigned values, returns the value itself.
   */
  T getAbsoluteValue() const {
    if (isNegative()) {
      return -mValue;
    }
    return mValue;
  }

  size_t printTo(Print& p) const final {
    size_t n = 0;

    const uint16_t scaleFactor = getScaleFactor();

    // Print negative sign if needed
    if (isNegative()) {
      n += p.print('-');
    }

    // Get absolute value and calculate integer part
    T absValue = getAbsoluteValue();
    uint32_t integer = absValue / scaleFactor;
    n += p.print(integer);

    // Print fractional part if needed
    if (scaleFactor != 1) {
      n += p.print('.');

      uint32_t fractional = absValue % scaleFactor;

      // Pad leading zeros in fractional part
      if (scaleFactor >= 1000 && fractional < 100) {
        n += p.print('0');
      }
      if (scaleFactor >= 100 && fractional < 10) {
        n += p.print('0');
      }
      n += p.print(fractional);
    }

    n += mUnit.print(p);
    return n;
  }

  const Unit& getUnit() const { return mUnit; }

  uint8_t getPrecision() const { return mPrecision; }

  static constexpr size_t getValueSize() { return sizeof(T); }

  int16_t getScaleFactor() const {
    return ValueItemConstants::factors[mPrecision];
  }

  T getMinValue() const { return mMin; }

  T getMaxValue() const { return mMax; }

 private:
  T mValue;
  const T mMin;
  const T mMax;
  const Unit mUnit;
  const uint8_t mPrecision;
};
