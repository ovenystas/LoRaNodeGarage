#include "AirTime.h"

#include <Arduino.h>

#include "RingBuffer.h"

void AirTime::update() { update(millis()); }

void AirTime::update(uint32_t time) {
  while (time >= (mLastUpdate_ms + MS_PER_MINUTE)) {
    mTime_ms -= mBuf.pop_back();
    mBuf.push_front(0);
    mLastUpdate_ms += MS_PER_MINUTE;
  }
}

void AirTime::update(uint32_t start, uint32_t end) {
  update(start);
  uint32_t splitTime = (end / MS_PER_MINUTE) * MS_PER_MINUTE;
  if (splitTime > start) {
    uint32_t td1 = splitTime - start;
    uint32_t td2 = end - splitTime;
    mTime_ms = mTime_ms + td1 + td2;
    uint16_t currentMinute = mBuf.pop_front();
    currentMinute += td1;
    mBuf.push_front(currentMinute);
    update(splitTime);
    mBuf.push_front(td2);
  } else {
    uint32_t td = end - start;
    mTime_ms += td;
    uint16_t currentMinute = mBuf.pop_front();
    currentMinute += td;
    mBuf.push_front(currentMinute);
  }
}

uint32_t AirTime::getTime_ms() {
  update();
  return mTime_ms;
}

uint16_t AirTime::getTime_ppm() {
  // Convert ms to ppm and round it.
  uint32_t time_ppm = (getTime_ms() * 10 + 18) / 36;

  if (time_ppm > UINT16_MAX) {
    time_ppm = UINT16_MAX;
  }

  return static_cast<uint16_t>(time_ppm);
}
