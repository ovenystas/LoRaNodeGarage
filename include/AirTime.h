#pragma once

#include <stdint.h>

#include "RingBuffer.h"
#include "Util.h"

class AirTime {
 public:
  explicit AirTime(uint16_t limit_ppm) : mLimit_ppm{limit_ppm} {
    mBuf.fill(0);
    // Initial push for first minute to be at pos zero in buf.
    mBuf.push_front(0);
  }

  uint32_t getTime_ms();   // For last hour
  uint16_t getTime_ppm();  // For last hour, 10000 ppm = 1 %

  bool isLimitReached() { return getTime_ppm() >= mLimit_ppm; };

  void update();
  void update(uint32_t start, uint32_t end);

 private:
  void update(uint32_t time);
  void addToCurrentMinute(uint32_t t);

  RingBuffer<uint16_t, Util::MINUTES_PER_HOUR> mBuf;
  uint32_t mLastUpdate_ms{};
  const uint16_t mLimit_ppm;
  uint32_t mTime_ms{};
};
