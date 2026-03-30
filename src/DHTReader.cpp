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

  // Try to read from the DHT sensor
  Serial.println(F("DHT: Attempting read..."));
  Serial.flush();
  
  bool readSuccess = false;
  
  if (mDht.read()) {
    readSuccess = true;
    mLastTemperature = mDht.readTemperature();
    mLastHumidity = mDht.readHumidity();
    Serial.print(F("DHT: Read success - T="));
    Serial.print(mLastTemperature);
    Serial.print(F("C, H="));
    Serial.print(mLastHumidity);
    Serial.println(F("%"));
  } else {
    Serial.println(F("DHT: Read failed"));
    Serial.flush();
  }
  
  if (!readSuccess) {
    mReadFailureCount++;
    if (mReadFailureCount == 1) {
      Serial.println(F("DHT: First read failure detected"));
    }
    mReadSuccessful = false;
  } else {
    mReadSuccessful = true;
    mReadFailureCount = 0;
  }
  
  return readSuccess;
}
