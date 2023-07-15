#pragma once

#include <NewPing.h>

#include "Component.h"
#include "ConfigItem.h"
#include "Sensor.h"
#include "Util.h"

using DistanceT = int16_t;  // cm

class DistanceSensor : public IComponent {
 public:
  virtual ~DistanceSensor() = default;

  DistanceSensor() = delete;

  DistanceSensor(uint8_t entityId, const char* name, NewPing& sonar)
      : mSensor{Sensor<DistanceT>(entityId, name, SensorDeviceClass::distance,
                                  Unit::Type::cm)},
        mSonar{sonar} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) final;

  uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  Sensor<DistanceT>& getSensor() { return mSensor; }

  uint8_t getValueMsg(uint8_t* buffer) final {
    return mSensor.getValueMsg(buffer);
  }

  size_t print(Stream& stream) final { return mSensor.print(stream); };

  size_t print(Stream& stream, uint8_t service) final {
    (void)stream;
    (void)service;
    return 0;
  };

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final;

  void setReported() final { mSensor.setReported(); }

  bool update() final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {3};

    ConfigItem<DistanceT> reportHysteresis = {
        ConfigItem<DistanceT>(0, 10, Unit::Type::cm)};

    ConfigItem<uint16_t> measureInterval = {
        ConfigItem<uint16_t>(1, 60, Unit::Type::s)};

    ConfigItem<uint16_t> reportInterval = {
        ConfigItem<uint16_t>(2, 60, Unit::Type::s)};
  };

  Sensor<DistanceT> mSensor;
  Config mConfig;
  NewPing& mSonar;
};
