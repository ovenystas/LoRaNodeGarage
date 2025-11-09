#include "AirTime.h"

#include <Arduino.h>
#include <assert.h>

#include "RingBuffer.h"

void AirTime::update() { update(millis()); }

void AirTime::update(uint32_t time) {
  if (time - mLastUpdate_ms >= MS_PER_HOUR) {
    mTime_ms = 0;
    mBuf.clear();
    mBuf.fill(0);
    while (time - mLastUpdate_ms >= MS_PER_HOUR) {
      mLastUpdate_ms += MS_PER_HOUR;
    }
  }

  while (time - mLastUpdate_ms >= MS_PER_MINUTE) {
    mTime_ms -= mBuf.pop_back();
    mBuf.push_front(0);
    mLastUpdate_ms += MS_PER_MINUTE;
  }
}

void AirTime::addToCurrentMinute(uint32_t t) {
  assert(t <= MS_PER_MINUTE);
  uint16_t currentMinute = mBuf.pop_front();
  assert(currentMinute + t <= MS_PER_MINUTE);
  currentMinute += t;
  mBuf.push_front(currentMinute);
}

void AirTime::update(uint32_t start, uint32_t end) {
  uint32_t t1 = start;

  do {
    update(t1);
    uint32_t t2 = (t1 / MS_PER_MINUTE + 1) * MS_PER_MINUTE;  // Minute boundary
    if (t2 > end) {
      t2 = end;
    }

    uint32_t td = t2 - t1;
    mTime_ms += td;
    addToCurrentMinute(td);
    t1 = t2;
  } while (t1 < end);
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
