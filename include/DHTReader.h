#pragma once

#include <DHT.h>
#include <stdint.h>

#include "Util.h"

class DHTReader {
 public:
  DHTReader() = delete;

  DHTReader(DHT& dht) : mDht{dht}, mLastReadTime{0xFFFFFFFFUL}, mLastTemperature{0.0f}, mLastHumidity{0.0f}, mReadSuccessful{false}, mReadFailureCount{0} {}

  /**
   * Initialize the DHT sensor. Must be called during setup.
   */
  void begin() { mDht.begin(); }

  /**
   * Update the DHT reader, reading from the sensor if the minimum
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
  // Minimum time between DHT reads (in milliseconds)
  // DHT sensors require at least 2000ms between reads
  static const uint16_t MIN_READ_INTERVAL_MS = 10000;  // 10 seconds to prevent sensor lockup

  DHT& mDht;
  uint32_t mLastReadTime;
  float mLastTemperature;
  float mLastHumidity;
  bool mReadSuccessful;
  uint8_t mReadFailureCount;
};
