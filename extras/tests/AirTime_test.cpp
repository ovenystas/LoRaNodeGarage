#include "AirTime.h"

#include <gtest/gtest.h>

#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"

using ::testing::Return;
using ::testing::ReturnPointee;

class AirTime_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    bufSerial.flush();
  }

  void TearDown() override { releaseArduinoMock(); }

  ArduinoMock* pArduinoMock;
};

TEST_F(AirTime_test, construct_airtime_shall_be_zero_and_limit_not_reached) {
  auto at = AirTime(10000);
  EXPECT_CALL(*pArduinoMock, millis()).WillRepeatedly(Return(0));

  EXPECT_EQ(at.getTime_ms(), 0);
  EXPECT_EQ(at.getTime_ppm(), 0);
  EXPECT_FALSE(at.isLimitReached());
}

TEST_F(AirTime_test,
       update_1ms_within_current_minute_shall_be_one_and_limit_not_reached) {
  auto at = AirTime(10000);
  EXPECT_CALL(*pArduinoMock, millis()).WillRepeatedly(Return(2));

  at.update(1, 2);

  EXPECT_EQ(at.getTime_ms(), 1);
  EXPECT_EQ(at.getTime_ppm(), 0);
  EXPECT_FALSE(at.isLimitReached());
}

TEST_F(
    AirTime_test,
    update_35999ms_then_1ms_within_current_minute_shall_be_10000ppm_and_limit_reached) {
  auto at = AirTime(10000);
  EXPECT_CALL(*pArduinoMock, millis()).WillRepeatedly(Return(36000));

  at.update(2, 36000);

  EXPECT_EQ(at.getTime_ms(), 35998);
  EXPECT_EQ(at.getTime_ppm(), 9999);
  EXPECT_FALSE(at.isLimitReached());

  EXPECT_CALL(*pArduinoMock, millis()).WillRepeatedly(Return(40001));

  at.update(40000, 40001);

  EXPECT_EQ(at.getTime_ms(), 35999);
  EXPECT_EQ(at.getTime_ppm(), 10000);
  EXPECT_TRUE(at.isLimitReached());
}

TEST_F(AirTime_test, update_1ms_shall_be_gone_after_1hour) {
  auto at = AirTime(10000);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(1))
      .WillOnce(Return(MS_PER_HOUR - 1))
      .WillOnce(Return(MS_PER_HOUR - 1))
      .WillOnce(Return(MS_PER_HOUR))
      .WillOnce(Return(MS_PER_HOUR));

  at.update(0, 1);

  EXPECT_EQ(at.getTime_ms(), 1);

  at.update();

  EXPECT_EQ(at.getTime_ms(), 1);

  at.update();

  EXPECT_EQ(at.getTime_ms(), 0);
}

TEST_F(
    AirTime_test,
    update_over_minute_boundary_shall_be_split_over_two_minutes_first_minute_gone_after_1hour) {
  auto at = AirTime(10000);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(MS_PER_MINUTE + 1))
      .WillOnce(Return(MS_PER_HOUR))
      .WillOnce(Return(MS_PER_HOUR + MS_PER_MINUTE));

  at.update(MS_PER_MINUTE - 2, MS_PER_MINUTE + 1);

  EXPECT_EQ(at.getTime_ms(), 3);
  EXPECT_EQ(at.getTime_ms(), 1);
  EXPECT_EQ(at.getTime_ms(), 0);
}

TEST_F(AirTime_test,
       update_of_time_over_two_minutes_shall_be_split_over_three_minutes) {
  auto at = AirTime(10000);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(MS_PER_MINUTE * 2 + 1))
      .WillOnce(Return(MS_PER_HOUR))
      .WillOnce(Return(MS_PER_HOUR + MS_PER_MINUTE))
      .WillOnce(Return(MS_PER_HOUR + MS_PER_MINUTE * 2));

  at.update(MS_PER_MINUTE - 1, MS_PER_MINUTE * 2 + 1);

  EXPECT_EQ(at.getTime_ms(), MS_PER_MINUTE + 2);
  EXPECT_EQ(at.getTime_ms(), MS_PER_MINUTE + 1);
  EXPECT_EQ(at.getTime_ms(), 1);
  EXPECT_EQ(at.getTime_ms(), 0);
}

TEST_F(
    AirTime_test,
    update_over_one_hour_10ms_each_minute_shall_result_in_600ms_then_decay_over_one_hour_shall_result_in_0ms) {
  auto at = AirTime(10000);
  uint32_t start = 100;
  uint32_t end = start + 10;
  uint32_t expected_ms = 0;
  EXPECT_CALL(*pArduinoMock, millis()).WillRepeatedly(ReturnPointee(&end));

  // Add values each minute over an hour
  for (uint8_t i = 0; i < MINUTES_PER_HOUR; i++) {
    Serial.flush();
    at.update(start, end);

    expected_ms += (end - start);
    EXPECT_EQ(at.getTime_ms(), expected_ms);

    start += MS_PER_MINUTE;
    end += MS_PER_MINUTE;
  }

  uint32_t t = 100 + MS_PER_HOUR - MS_PER_MINUTE;
  EXPECT_CALL(*pArduinoMock, millis()).WillRepeatedly(ReturnPointee(&t));

  EXPECT_EQ(at.getTime_ms(), 600);

  // Add no values over an hour to decay airtime
  for (uint8_t i = 0; i < MINUTES_PER_HOUR; i++) {
    Serial.flush();
    at.update();

    t += MS_PER_MINUTE;

    expected_ms -= 10;
    EXPECT_EQ(at.getTime_ms(), expected_ms);
  }

  EXPECT_EQ(at.getTime_ms(), 0);
}

TEST_F(
    AirTime_test,
    multiple_updates_during_same_minute_shall_add_them_in_the_same_slot_and_be_gone_after_1hour) {
  auto at = AirTime(10000);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(510))
      .WillOnce(Return(MS_PER_HOUR - 1))
      .WillOnce(Return(MS_PER_HOUR - 1))
      .WillOnce(Return(MS_PER_HOUR))
      .WillOnce(Return(MS_PER_HOUR));

  at.update(100, 110);
  at.update(200, 210);
  at.update(300, 310);
  at.update(400, 410);
  at.update(500, 510);

  // 5 updates adds up to 50 ms in current minute.
  EXPECT_EQ(at.getTime_ms(), 50);

  at.update();

  // Still 50 ms since it is 1 ms left to one hour.
  EXPECT_EQ(at.getTime_ms(), 50);

  at.update();

  // One hour completed, now 0 ms.
  EXPECT_EQ(at.getTime_ms(), 0);
}

TEST_F(
    AirTime_test,
    for_airtime_ppm_above_UINT16_MAX_getTime_ppm_shall_be_limited_to_UINT16_MAX) {
  auto at = AirTime(10000);
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(235924))
      .WillOnce(Return(235925))
      .WillOnce(Return(235928))
      .WillOnce(Return(300000))
      .WillOnce(Return(400000));

  Serial.flush();
  at.update(0, 235924);
  EXPECT_EQ(at.getTime_ppm(), UINT16_MAX - 1);

  Serial.flush();
  at.update(235924, 235925);
  EXPECT_EQ(at.getTime_ppm(), UINT16_MAX);

  Serial.flush();
  at.update(235925, 235928);
  EXPECT_EQ(at.getTime_ppm(), UINT16_MAX);

  Serial.flush();
  at.update(235928, 300000);
  EXPECT_EQ(at.getTime_ppm(), UINT16_MAX);

  Serial.flush();
  at.update(300000, 400000);
  EXPECT_EQ(at.getTime_ppm(), UINT16_MAX);
}

TEST_F(AirTime_test, update_during_millis_rollover) {
  auto at = AirTime(10000);
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(1));

  at.update(UINT32_MAX, 1);

  EXPECT_EQ(at.getTime_ms(), 2);
}
