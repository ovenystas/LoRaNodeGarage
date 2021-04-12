/*
 *  Created on: 22 mars 2021
 *      Author: oveny
 */

#pragma once

static const String UnitName[] = { "", "C", "F", "K", "%", "km", "m", "dm",
    "cm", "mm", "um" };

class Unit {
public:
  typedef enum {
    None,
    DegreeC,
    DegreeF,
    DegreeK,
    Percent,
    KiloMeter,
    Meter,
    DeciMeter,
    CentiMeter,
    MilliMeter,
    MicroMeter,
  } TypeE;

  Unit(TypeE type) :
    mType { type } {
  }

  virtual ~Unit() = default;

  TypeE getType() const {
    return mType;
  }

  const String& getName() const {
    return UnitName[mType];
  }

protected:
  const TypeE mType = { None };
};
