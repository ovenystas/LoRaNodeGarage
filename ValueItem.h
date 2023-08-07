#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "Unit.h"

#define IS_SIGNED_TYPE(type) (type(-1) < type(0))

namespace ValueItemConstants {
static const uint16_t factors[4] = {1, 10, 100, 1000};
}

template <class T>
class ValueItem {
 public:
  ValueItem(Unit::Type unitType = Unit::Type::none, uint8_t precision = 0,
            T value = 0)
      : mValue{value},
        mUnit{Unit(unitType)},
        mPrecision{precision > 3 ? static_cast<uint8_t>(3) : precision} {}

  T getValue() const { return mValue; }

  void setValue(T value = {}) { mValue = value; }

  bool isSigned() const { return IS_SIGNED_TYPE(T); }

  size_t print(Stream& stream) const {
    size_t n = 0;

    const uint16_t scaleFactor = ValueItemConstants::factors[mPrecision];
    if (scaleFactor == 1) {
      n += stream.print(mValue);
    } else {
      // cppcheck-suppress unsignedLessThanZero
      // cppcheck-suppress unmatchedSuppression
      if (IS_SIGNED_TYPE(T) && mValue < 0) {
        n += stream.print('-');
      }

      uint32_t integer;
      // cppcheck-suppress unsignedLessThanZero
      // cppcheck-suppress unmatchedSuppression
      if (IS_SIGNED_TYPE(T) && mValue < 0) {
        integer = -(mValue / scaleFactor);
      } else {
        integer = mValue / scaleFactor;
      }

      n += stream.print(integer);

      n += stream.print('.');

      uint32_t fractional;
      // cppcheck-suppress unsignedLessThanZero
      // cppcheck-suppress unmatchedSuppression
      if (IS_SIGNED_TYPE(T) && mValue < 0) {
        fractional = -(mValue % scaleFactor);
      } else {
        fractional = mValue % scaleFactor;
      }

      if (scaleFactor >= 1000 && fractional < 100) {
        n += stream.print('0');
      }
      if (scaleFactor >= 100 && fractional < 10) {
        n += stream.print('0');
      }
      n += stream.print(fractional);
    }

    n += mUnit.print(stream);
    return n;
  }

  const Unit& getUnit() const { return mUnit; }

  uint8_t getPrecision() const { return mPrecision; }

  size_t getValueSize() const { return sizeof(T); }

  int16_t getScaleFactor() const {
    return ValueItemConstants::factors[mPrecision];
  }

 private:
  T mValue;
  const Unit mUnit;
  const uint8_t mPrecision;
};
