#pragma once

#include "BinarySensor.h"
#include "Component.h"
#include "ConfigItem.h"
#include "HeightSensor.h"

class PresenceBinarySensor : public IComponent {
 public:
  virtual ~PresenceBinarySensor() = default;

  PresenceBinarySensor() = delete;

  PresenceBinarySensor(uint8_t entityId, const char* name,
                       Sensor<HeightT>& heightSensor)
      : mBinarySensor{BinarySensor(entityId, name,
                                   BinarySensorDeviceClass::presence)},
        mHeightSensor{heightSensor} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) final;

  uint8_t getDiscoveryMsg(uint8_t* buffer) final;

  uint8_t getEntityId() const final { return mBinarySensor.getEntityId(); }

  uint8_t getValueMsg(uint8_t* buffer) final {
    return mBinarySensor.getValueMsg(buffer);
  }

  size_t print(Stream& stream) final { return mBinarySensor.print(stream); };

  size_t print(Stream& stream, uint8_t service) final {
    (void)stream;
    (void)service;
    return 0;
  };

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final;

  void setReported() final { mBinarySensor.setReported(); }

  bool update() final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {4};

    ConfigItem<HeightT> lowLimit = {
        ConfigItem<HeightT>(0, 180, Unit::Type::cm, 0)};

    ConfigItem<HeightT> highLimit = {
        ConfigItem<HeightT>(1, 200, Unit::Type::cm, 0)};

    ConfigItem<uint16_t> minStableTime = {
        ConfigItem<uint16_t>(2, 10000, Unit::Type::ms, 0)};

    ConfigItem<uint16_t> reportInterval = {
        ConfigItem<uint16_t>(3, 60, Unit::Type::s, 0)};
  };

  BinarySensor mBinarySensor;
  Config mConfig;
  Sensor<HeightT>& mHeightSensor;
  uint32_t mLastChangedTime = {};
  bool mStableState = {};
};
