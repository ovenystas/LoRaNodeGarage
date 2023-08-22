#pragma once

#include <Arduino.h>
#include <Printable.h>
#include <stdint.h>

#include "Unit.h"

#define IS_SIGNED_TYPE(type) (type(-1) < type(0))

namespace ValueItemConstants {
static const uint16_t factors[4] = {1, 10, 100, 1000};
}

template <class T>
class ValueItem : public Printable {
 public:
  ValueItem(Unit::Type unitType = Unit::Type::none, uint8_t precision = 0,
            T value = 0)
      : mValue{value},
        mUnit{Unit(unitType)},
        mPrecision{precision > 3 ? static_cast<uint8_t>(3) : precision} {}

  virtual ~ValueItem() {}

  T getValue() const { return mValue; }

  void setValue(T value = {}) { mValue = value; }

  static constexpr bool isSigned() { return IS_SIGNED_TYPE(T); }

  size_t printTo(Print& p) const final {
    size_t n = 0;

    const uint16_t scaleFactor = ValueItemConstants::factors[mPrecision];
    // cppcheck-suppress unsignedLessThanZero
    // cppcheck-suppress unmatchedSuppression
    if (IS_SIGNED_TYPE(T) && mValue < 0) {
      n += p.print('-');
    }

    uint32_t integer;
    // cppcheck-suppress unsignedLessThanZero
    // cppcheck-suppress unmatchedSuppression
    if (IS_SIGNED_TYPE(T) && mValue < 0) {
      integer = -(mValue / scaleFactor);
    } else {
      integer = mValue / scaleFactor;
    }

    n += p.print(integer);

    if (scaleFactor != 1) {
      n += p.print('.');

      uint32_t fractional;
      // cppcheck-suppress unsignedLessThanZero
      // cppcheck-suppress unmatchedSuppression
      if (IS_SIGNED_TYPE(T) && mValue < 0) {
        fractional = -(mValue % scaleFactor);
      } else {
        fractional = mValue % scaleFactor;
      }

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

 private:
  T mValue;
  const Unit mUnit;
  const uint8_t mPrecision;
};
