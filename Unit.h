/*
 *  Created on: 22 mars 2021
 *      Author: oveny
 */

#pragma once

#include <stdint.h>

static const char UnitName[][3] = {"",   "C",  "F",  "K",  "%", "km", "m",
                                   "dm", "cm", "mm", "um", "s", "ms"};

class Unit {
 public:
  enum class Type {
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

  virtual ~Unit() = default;

  inline Type getType() const { return mType; }

  inline const char* getName() const {
    return UnitName[static_cast<uint8_t>(mType)];
  }

 private:
  const Type mType = {Type::none};
};
