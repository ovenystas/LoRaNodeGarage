#pragma once

#include <Adafruit_AHTX0.h>
#include <stdint.h>

#include "Util.h"

class AHTReader {
 public:
  AHTReader() = delete;

  AHTReader(Adafruit_AHTX0& aht) : mAht{aht} {}

  /**
   * Initialize the AHT sensor. Must be called during setup.
   */
  void begin();

  /**
   * Update the AHT reader, reading from the sensor if the minimum
   * interval (2000ms) has elapsed since the last read.
   *
   * @return true if a new read was performed, false otherwise
   */
  bool update();

  /**
   * Get the last successfully read temperature value.
   * @return Temperature in degrees Celsius
   */
  float getTemperature() const { return mLastTemperature; }

  /**
   * Get the last successfully read humidity value.
   * @return Humidity in percent (0-100)
   */
  float getHumidity() const { return mLastHumidity; }

  /**
   * Check if the last read was successful.
   * @return true if the last read had a valid checksum
   */
  bool isReadSuccessful() const { return mReadSuccessful; }

 private:
  static const uint16_t MIN_READ_INTERVAL_MS = 2000;

  Adafruit_AHTX0& mAht;
  uint32_t mLastReadTime{-MIN_READ_INTERVAL_MS};
  float mLastTemperature{0.0f};
  float mLastHumidity{0.0f};
  bool mReadSuccessful{false};
};
