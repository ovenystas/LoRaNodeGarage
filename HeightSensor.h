#pragma once

#include <stdint.h>

#include "Component.h"
#include "ConfigItem.h"
#include "DistanceSensor.h"
#include "EeAdressMap.h"
#include "Sensor.h"
#include "Unit.h"
#include "Util.h"

using HeightT = int16_t;  // cm

class HeightSensor : public IComponent {
 public:
  virtual ~HeightSensor() = default;

  HeightSensor() = delete;

  HeightSensor(uint8_t entityId, const char* name,
               Sensor<DistanceT>& distanceSensor)
      : mSensor{Sensor<HeightT>(entityId, name, SensorDeviceClass::distance,
                                Unit::Type::cm)},
        mDistanceSensor{distanceSensor} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigItemValues(ConfigItemValueT* items,
                              uint8_t length) const final;

  void getDiscoveryItem(DiscoveryItemT* item) const final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  Sensor<HeightT>& getSensor() { return mSensor; }

  void getValueItem(ValueItemT* item) const final {
    return mSensor.getValueItem(item);
  }

  bool isReportDue() const final { return mSensor.isReportDue(); }

  void loadConfigValues();
  void saveConfigValues() const;

  size_t print(Stream& stream) const final { return mSensor.print(stream); };

  size_t print(Stream& stream, uint8_t service) const final {
    (void)stream;
    (void)service;
    return 0;
  };

  bool setConfigItemValues(const ConfigItemValueT* items, uint8_t length) final;

  void setReported() final { mSensor.setReported(); }

  bool update() final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    const uint8_t numberOfConfigItems = {4};

    ConfigItem<HeightT> reportHysteresis = {ConfigItem<HeightT>(
        0, EE_ADDRESS_CONFIG_HEIGHTSENSOR_0, 10, Unit::Type::cm)};

    ConfigItem<uint16_t> reportInterval = {ConfigItem<uint16_t>(
        1, EE_ADDRESS_CONFIG_HEIGHTSENSOR_1, 60, Unit::Type::s)};

    ConfigItem<uint16_t> stableTime = {ConfigItem<uint16_t>(
        2, EE_ADDRESS_CONFIG_HEIGHTSENSOR_0, 5000, Unit::Type::ms)};

    ConfigItem<HeightT> zeroValue = {ConfigItem<HeightT>(
        3, EE_ADDRESS_CONFIG_HEIGHTSENSOR_0, 60, Unit::Type::cm)};
  };

  Sensor<HeightT> mSensor;
  Config mConfig;
  Sensor<DistanceT>& mDistanceSensor;
};
