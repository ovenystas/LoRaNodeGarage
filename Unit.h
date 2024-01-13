/*
 *  Created on: 22 mars 2021
 *      Author: oveny
 */

#pragma once

#include <Print.h>
#include <stdint.h>

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

  const char* name() const {
    return UnitConstants::UnitName[static_cast<uint8_t>(mType)];
  }

  size_t print(Print& printer) const {
    size_t n = 0;
    if (mType != Unit::Type::none) {
      n += printer.print(' ');
      n += printer.print(name());
    }
    return n;
  }

 private:
  const Type mType{Type::none};
};
