#pragma once

#include <Print.h>
#include <stdint.h>

#ifdef ARDUINO
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

namespace UnitConstants {
static const char UnitName[][4] PROGMEM = {
    "", "°C", "°F", "K", "%", "km", "m", "dm", "cm", "mm", "μm", "s", "ms"};
}

class Unit {
 public:
  enum class Type : uint8_t {
    none,
    C,
    F,
    K,
    percent,
    km,
    m,
    dm,
    cm,
    mm,
    um,
    s,
    ms,
  };

  explicit Unit(Type type) : mType{type} {}

  Type type() const { return mType; }

  size_t print(Print& printer) const {
    size_t n = 0;
    if (mType != Unit::Type::none) {
      n += printer.print(reinterpret_cast<const __FlashStringHelper*>(
          UnitConstants::UnitName[static_cast<uint8_t>(mType)]));
    }
    return n;
  }

 private:
  const Type mType{Type::none};
};
