/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

/*
 *
 */
class BinarySensor {
public:
  BinarySensor() = default;
  virtual ~BinarySensor() = default;

  virtual bool update() = 0;
  virtual bool getState() const { return mState; }

protected:
  bool mState = {};
};
