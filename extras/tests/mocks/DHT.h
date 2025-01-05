#pragma once

#include <gmock/gmock.h>

class DHT {
 public:
  virtual float readHumidity(bool force = false) = 0;
  virtual float readTemperature(bool S = false, bool force = false) = 0;
  virtual bool read(bool force = false) = 0;
};

class DHTMock : public DHT {
 public:
  virtual ~DHTMock() = default;
  MOCK_METHOD(float, readHumidity, (bool));
  MOCK_METHOD(float, readTemperature, (bool, bool));
  MOCK_METHOD(bool, read, (bool));
};
