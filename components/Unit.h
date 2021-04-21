/*
 *  Created on: 22 mars 2021
 *      Author: oveny
 */

#pragma once

static const String UnitName[] = { "", "C", "F", "K", "%", "km", "m", "dm",
    "cm", "mm", "um" };

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
  };

  Unit(Type type) :
    mType { type } {
  }

  virtual ~Unit() = default;

  Type getType() const {
    return mType;
  }

  const String& getName() const {
    return UnitName[static_cast<uint8_t>(mType)];
  }

private:
  const Type mType = { Type::none };
};
