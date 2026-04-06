#pragma once

#include <Adafruit_I2CDevice.h>
#include <Wire.h>

#define AHT20_I2CADDR_DEFAULT 0x38    ///< AHT default i2c address
#define AHT20_I2CADDR_ALTERNATE 0x39  ///< AHT alternate i2c address

#define AHT20_CMD_CALIBRATE 0xE1  ///< Calibration command
#define AHT20_CMD_TRIGGER 0xAC    ///< Trigger reading command
#define AHT20_CMD_SOFTRESET 0xBA  ///< Soft reset command

#define AHT20_STATUS_BUSY 0x80        ///< Status bit for busy
#define AHT20_STATUS_CALIBRATED 0x08  ///< Status bit for calibrated

/**
 * @brief   Class that stores state and functions for interacting with
 *          the AHT20 I2C Temperature/Humidity sensor
 */
class AHT20 {
 public:
  /**
   * @brief   Instantiates a new AHT20 class
   * @param   wire
   *          The Wire object to be used for I2C connections.
   * @param   i2c_address
   *          The I2C address used to communicate with the sensor
   */
  AHT20(TwoWire *wire = &Wire, uint8_t i2c_address = AHT20_I2CADDR_DEFAULT)
      : i2c_dev{Adafruit_I2CDevice(i2c_address, wire)} {}

  /**
   * @brief   Sets up the hardware and initializes I2C
   * @returns True if initialization was successful, otherwise false
   */
  bool begin();

  /**
   * @brief   Gets the status (first byte) from AHT20
   * @returns 8 bits of status data, or 0xFF if failed
   */
  uint8_t getStatus(void);

  /**
   * @brief   Gets the humidity and temperature measurements from AHT20
   * @param   humidity Pointer to float variable to store humidity value
   * @param   temperature Pointer to float variable to store temperature value
   * @returns True if the measurements were read successfully, otherwise false
   */
  bool getMeasurements(float *humidity, float *temperature);

 private:
  Adafruit_I2CDevice i2c_dev;  ///< I2C bus interface
};
