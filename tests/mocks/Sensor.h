#pragma once

#include <gmock/gmock.h>
#include <stdbool.h>
#include <stdint.h>

#include "Sensor.h"

template <class T>
class SensorMock : public virtual ISensor<T> {
 public:
  virtual ~SensorMock() = default;
  MOCK_METHOD(T, getValue, (), (const, override));
  MOCK_METHOD(bool, update, (), (override));
  MOCK_METHOD(uint8_t, getConfigItemValuesMsg, (uint8_t*), (override));
  MOCK_METHOD(void, callService, (uint8_t), (override));
  MOCK_METHOD(bool, hasService, (), (override));
  MOCK_METHOD(bool, setConfigs, (uint8_t, const uint8_t*), (override));
  MOCK_METHOD(void, print, (Stream&, uint8_t), (override));
  MOCK_METHOD(void, print, (Stream&), (override));
  MOCK_METHOD(void, setReported, (), (override));
  MOCK_METHOD(uint32_t, timeSinceLastReport, (), (const, override));
  MOCK_METHOD(uint8_t, getEntityId, (), (const, override));
  MOCK_METHOD(const char*, getName, (), (const, override));
  MOCK_METHOD(Component::Type, getComponentType, (), (const, override));
  MOCK_METHOD(SensorDeviceClass, getDeviceClass, (), (const, override));
  MOCK_METHOD(const char*, getUnitName, (), (const, override));
  MOCK_METHOD(uint8_t, getDiscoveryMsg, (uint8_t*), (override));
  MOCK_METHOD(uint8_t, getValueMsg, (uint8_t*), (override));
  MOCK_METHOD(HeightT, absDiffLastReportedValue, (), (const, override));
  MOCK_METHOD(void, setValue, (T), (override));
  MOCK_METHOD(Unit::Type, getUnitType, (), (const, override));
};
