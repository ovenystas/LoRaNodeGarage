#pragma once

/**
 * @brief EEPROM address map.
 */
#define EE_ADDRESS_CONFIG_MAGIC (0x0030)

#define EE_ADDRESS_DISTANCE_SENSOR (0x0040)
#define EE_ADDRESS_GARAGE_COVER (0x0060)
#define EE_ADDRESS_HEIGHT_SENSOR (0x0080)
#define EE_ADDRESS_HUMIDITY_SENSOR (0x00A0)
#define EE_ADDRESS_PRESENCE_BINARY_SENSOR (0x00C0)
#define EE_ADDRESS_TEMPERATURE_SENSOR (0x00E0)

/**
 * @brief Detailed EEPROM address map for GarageCover.
 */
#define EE_ADDRESS_CONFIG_GARAGE_COVER_0 EE_ADDRESS_GARAGE_COVER

/**
 * @brief Detailed EEPROM address map for DistanceSensor.
 */
#define EE_ADDRESS_CONFIG_DISTANCE_SENSOR_0 EE_ADDRESS_DISTANCE_SENSOR
#define EE_ADDRESS_CONFIG_DISTANCE_SENSOR_1 \
  (EE_ADDRESS_CONFIG_DISTANCE_SENSOR_0 + sizeof(DistanceT) + 1)
#define EE_ADDRESS_CONFIG_DISTANCE_SENSOR_2 \
  (EE_ADDRESS_CONFIG_DISTANCE_SENSOR_1 + sizeof(uint16_t) + 1)
#define EE_ADDRESS_CONFIG_DISTANCE_SENSOR_3 \
  (EE_ADDRESS_CONFIG_DISTANCE_SENSOR_2 + sizeof(uint16_t) + 1)

/**
 * @brief Detailed EEPROM address map for HeightSensor.
 */
#define EE_ADDRESS_CONFIG_HEIGHT_SENSOR_0 EE_ADDRESS_HEIGHT_SENSOR
#define EE_ADDRESS_CONFIG_HEIGHT_SENSOR_1 \
  (EE_ADDRESS_CONFIG_HEIGHT_SENSOR_0 + sizeof(HeightT) + 1)
#define EE_ADDRESS_CONFIG_HEIGHT_SENSOR_2 \
  (EE_ADDRESS_CONFIG_HEIGHT_SENSOR_1 + sizeof(uint16_t) + 1)
#define EE_ADDRESS_CONFIG_HEIGHT_SENSOR_3 \
  (EE_ADDRESS_CONFIG_HEIGHT_SENSOR_2 + sizeof(uint16_t) + 1)

/**
 * @brief Detailed EEPROM address map for HumiditySensor.
 */
#define EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_0 EE_ADDRESS_HUMIDITY_SENSOR
#define EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_1 \
  (EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_0 + sizeof(HumidityT) + 1)
#define EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_2 \
  (EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_1 + sizeof(uint16_t) + 1)
#define EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_3 \
  (EE_ADDRESS_CONFIG_HUMIDITY_SENSOR_2 + sizeof(uint16_t) + 1)

/**
 * @brief Detailed EEPROM address map for PresenceBinarySensor.
 */
#define EE_ADDRESS_CONFIG_PRESENCE_BINARY_SENSOR_0 \
  EE_ADDRESS_PRESENCE_BINARY_SENSOR
#define EE_ADDRESS_CONFIG_PRESENCE_BINARY_SENSOR_1 \
  (EE_ADDRESS_CONFIG_PRESENCE_BINARY_SENSOR_0 + sizeof(HeightT) + 1)
#define EE_ADDRESS_CONFIG_PRESENCE_BINARY_SENSOR_2 \
  (EE_ADDRESS_CONFIG_PRESENCE_BINARY_SENSOR_1 + sizeof(HeightT) + 1)
#define EE_ADDRESS_CONFIG_PRESENCE_BINARY_SENSOR_3 \
  (EE_ADDRESS_CONFIG_PRESENCE_BINARY_SENSOR_2 + sizeof(uint16_t) + 1)

/**
 * @brief Detailed EEPROM address map for TemperatureSensor.
 */
#define EE_ADDRESS_CONFIG_TEMPERATURE_SENSOR_0 EE_ADDRESS_TEMPERATURE_SENSOR
#define EE_ADDRESS_CONFIG_TEMPERATURE_SENSOR_1 \
  (EE_ADDRESS_CONFIG_TEMPERATURE_SENSOR_0 + sizeof(TemperatureT) + 1)
#define EE_ADDRESS_CONFIG_TEMPERATURE_SENSOR_2 \
  (EE_ADDRESS_CONFIG_TEMPERATURE_SENSOR_1 + sizeof(uint16_t) + 1)
#define EE_ADDRESS_CONFIG_TEMPERATURE_SENSOR_3 \
  (EE_ADDRESS_CONFIG_TEMPERATURE_SENSOR_2 + sizeof(uint16_t) + 1)
