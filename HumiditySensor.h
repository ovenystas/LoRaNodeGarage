#pragma once

#include <DHT.h>
#include <stdint.h>

#include "Component.h"
#include "ConfigItem.h"
#include "EeAdressMap.h"
#include "Sensor.h"
#include "Util.h"

using HumidityT = uint8_t;  // %

class HumiditySensor : public IComponent {
 public:
  virtual ~HumiditySensor() = default;

  HumiditySensor() = delete;

  HumiditySensor(uint8_t entityId, const char* name, DHT& dht)
      : mSensor{Sensor<HumidityT>(entityId, name, SensorDeviceClass::humidity,
                                  Unit::Type::percent)},
        mDht{dht} {}

  void callService(uint8_t service) final { (void)service; }

  uint8_t getConfigItemValues(ConfigItemValueT* items,
                              uint8_t length) const final;

  void getDiscoveryItem(DiscoveryItemT* item) const final;

  uint8_t getEntityId() const final { return mSensor.getEntityId(); }

  void getValueItem(ValueItemT* item) const final {
    return mSensor.getValueItem(item);
  }

  bool isReportDue() const final { return mSensor.isReportDue(); }

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

    ConfigItem<HumidityT> reportHysteresis = {ConfigItem<HumidityT>(
        0, EE_ADDRESS_CONFIG_HUMIDITYSENSOR_0, 2, Unit::Type::percent)};

    ConfigItem<uint16_t> measureInterval = {ConfigItem<uint16_t>(
        1, EE_ADDRESS_CONFIG_HUMIDITYSENSOR_1, 60, Unit::Type::s)};

    ConfigItem<uint16_t> reportInterval = {ConfigItem<uint16_t>(
        2, EE_ADDRESS_CONFIG_HUMIDITYSENSOR_2, 60, Unit::Type::s)};

    ConfigItem<HumidityT> compensation = {ConfigItem<HumidityT>(
        3, EE_ADDRESS_CONFIG_HUMIDITYSENSOR_3, 0, Unit::Type::percent)};
  };

  Sensor<HumidityT> mSensor;
  Config mConfig;
  DHT& mDht;
};
