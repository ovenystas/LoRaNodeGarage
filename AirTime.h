#pragma once

#include <stdint.h>

#include "RingBuffer.h"

#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define MS_PER_MINUTE (SECONDS_PER_MINUTE * 1000)
#define MS_PER_HOUR (MINUTES_PER_HOUR * MS_PER_MINUTE)

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

  RingBuffer<uint16_t, MINUTES_PER_HOUR> mBuf;
  uint32_t mLastUpdate_ms{};
  const uint16_t mLimit_ppm;
  uint32_t mTime_ms{};
};
