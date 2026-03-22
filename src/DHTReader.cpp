#include "DHTReader.h"

#include <Arduino.h>

// TODO: Replace this ugly workaround with something better
#ifndef HAVE_HWSERIAL0
#include "BufferSerial.h"
extern BufferSerial Serial;
#endif

bool DHTReader::update() {
  uint32_t now = millis();
  
  // Check if minimum interval has elapsed since last read
  if (now - mLastReadTime < MIN_READ_INTERVAL_MS) {
    return false;
  }

  mLastReadTime = now;

  // Attempt to read from the DHT sensor
  bool retVal = false;
  if (mDht.read()) {
    mLastTemperature = mDht.readTemperature();
    mLastHumidity = mDht.readHumidity();
    mReadSuccessful = true;
    mReadFailureCount = 0;
    retVal = true;
  } else {
    // Read failed (checksum error)
    mReadFailureCount++;
    Serial.print(F("DHT read FAILED ("));
    Serial.print(mReadFailureCount);
    Serial.println(F(")"));
    
    // Mark as unsuccessful only if we have had consecutive failures
    if (mReadFailureCount > 2) {
      mReadSuccessful = false;
    }
  }
  return retVal;
}
