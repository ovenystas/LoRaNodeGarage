#include "AHTReader.h"

#include <Arduino.h>

// TODO: Replace this ugly workaround with something better
#ifndef HAVE_HWSERIAL0
#include "BufferSerial.h"
extern BufferSerial Serial;
#endif

void AHTReader::begin() {
  if (!mAht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) {
      delay(10);
    }
  }
  Serial.println("AHT20 found");
}

bool AHTReader::update() {
  uint32_t now = millis();

  // Check if minimum interval has elapsed since last read
  if (now - mLastReadTime < MIN_READ_INTERVAL_MS) {
    return false;
  }

  mLastReadTime = now;

  sensors_event_t humidity, temp;

  if (mAht.getEvent(&humidity, &temp)) {
    mReadSuccessful = true;
    mLastTemperature = temp.temperature;
    mLastHumidity = humidity.relative_humidity;
  } else {
    mReadSuccessful = false;
    Serial.println(F("\nAHT: Read failed"));
  }

  return mReadSuccessful;
}
