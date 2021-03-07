/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include "Util.h"

/*
 *
 */
template<class T>
class Sensor {
public:
  Sensor() = default;
  virtual ~Sensor() = default;

  virtual bool update() = 0;
  virtual T getValue() const {
    return mValue;
  }
  void setReported() {
    mLastReportTime = seconds();
    mLastReportedValue = mValue;
  }

protected:
  T mValue = { };
  T mLastReportedValue = { };
  uint32_t mLastReportTime = { }; // s
};
