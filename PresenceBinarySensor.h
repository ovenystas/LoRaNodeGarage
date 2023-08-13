#pragma once

#include "BinarySensor.h"
#include "Component.h"
#include "ConfigItem.h"
#include "EeAdressMap.h"
#include "HeightSensor.h"

namespace PresenceBinarySensorConstants {
static const HeightT CONFIG_LOW_LIMIT_DEFAULT = 180;
static const HeightT CONFIG_HIGH_LIMIT_DEFAULT = 200;
static const uint16_t CONFIG_MIN_STABLE_TIME_DEFAULT = 10000;
static const uint16_t CONFIG_REPORT_INTERVAL_DEFAULT = 300;
}  // namespace PresenceBinarySensorConstants

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

  uint8_t getConfigItemValues(ConfigItemValueT* items,
                              uint8_t length) const final;

  void getDiscoveryItem(DiscoveryItemT* item) const final;

  uint8_t getEntityId() const final { return mBinarySensor.getEntityId(); }

  void getValueItem(ValueItemT* item) const final {
    return mBinarySensor.getValueItem(item);
  }

  bool isReportDue() const final { return mBinarySensor.isReportDue(); }

  size_t print(Stream& stream) const final {
    return mBinarySensor.print(stream);
  };

  size_t print(Stream& stream, uint8_t service) const final {
    (void)stream;
    (void)service;
    return 0;
  };

  void loadConfigValues() final;

  bool setConfigItemValues(const ConfigItemValueT* items, uint8_t length) final;

  void setReported() final { mBinarySensor.setReported(); }

  bool update() final;

 private:
  struct Config {
    // cppcheck-suppress unusedStructMember
    static const uint8_t numberOfConfigItems = {4};

    ConfigItem<HeightT> lowLimit = {ConfigItem<HeightT>(
        0, EE_ADDRESS_CONFIG_PRESENCEBINARYSENSOR_0,
        PresenceBinarySensorConstants::CONFIG_LOW_LIMIT_DEFAULT,
        Unit::Type::cm)};

    ConfigItem<HeightT> highLimit = {ConfigItem<HeightT>(
        1, EE_ADDRESS_CONFIG_PRESENCEBINARYSENSOR_1,
        PresenceBinarySensorConstants::CONFIG_HIGH_LIMIT_DEFAULT,
        Unit::Type::cm)};

    ConfigItem<uint16_t> minStableTime = {ConfigItem<uint16_t>(
        2, EE_ADDRESS_CONFIG_PRESENCEBINARYSENSOR_2,
        PresenceBinarySensorConstants::CONFIG_MIN_STABLE_TIME_DEFAULT,
        Unit::Type::ms)};

    ConfigItem<uint16_t> reportInterval = {ConfigItem<uint16_t>(
        3, EE_ADDRESS_CONFIG_PRESENCEBINARYSENSOR_3,
        PresenceBinarySensorConstants::CONFIG_REPORT_INTERVAL_DEFAULT,
        Unit::Type::s)};
  };

  BinarySensor mBinarySensor;
  Config mConfig;
  Sensor<HeightT>& mHeightSensor;
  uint32_t mLastChangedTime{};
  bool mStableState{};
};
