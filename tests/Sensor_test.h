#include "Sensor.h"

template <class T>
class SensorChild : public Sensor<T> {
 public:
  explicit SensorChild(uint8_t entityId) : Sensor<T>(entityId) {}

  SensorChild(uint8_t entityId, const char* name,
              Unit::Type unitType = Unit::Type::none, uint8_t precision = 0)
      : Sensor<T>(entityId, name, SensorDeviceClass::none, unitType,
                  precision) {}

  virtual ~SensorChild() = default;

  bool update() override { return false; }

  uint8_t getConfigItemValuesMsg(uint8_t* buffer) final {
    buffer[0] = 0;
    return 0;
  }

  bool setConfigs(uint8_t numberOfConfigs, const uint8_t* buffer) final {
    return false;
  }
};

class SensorInt8_test : public ::testing::Test {
 protected:
  SensorChild<int8_t> sc = SensorChild<int8_t>(108, "Sensor8");
};

class SensorInt16_test : public ::testing::Test {
 protected:
  SensorChild<int16_t> sc = SensorChild<int16_t>(116, "Sensor16");
};

class SensorInt32_test : public ::testing::Test {
 protected:
  SensorChild<int32_t> sc = SensorChild<int32_t>(132, "Sensor32");
};
