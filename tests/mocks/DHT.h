#pragma once

#include <gmock/gmock.h>

class DHT {
 public:
  virtual float readHumidity(bool force = false) = 0;
};

class DHTMock : public DHT {
 public:
  virtual ~DHTMock() = default;
  MOCK_METHOD(float, readHumidity, (bool));
};
