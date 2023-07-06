#include "../Util.h"

#include <gtest/gtest.h>

#include "mocks/Arduino.h"

using ::testing::Return;

TEST(Util_test, seconds) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis())
      .WillOnce(Return(0))
      .WillOnce(Return(999))
      .WillOnce(Return(1000))
      .WillOnce(Return(1999))
      .WillOnce(Return(UINT32_MAX));
  EXPECT_EQ(seconds(), 0);
  EXPECT_EQ(seconds(), 0);
  EXPECT_EQ(seconds(), 1);
  EXPECT_EQ(seconds(), 1);
  EXPECT_EQ(seconds(), UINT32_MAX / 1000);
  releaseArduinoMock();
}

TEST(Util_test, lowWord) { EXPECT_EQ(lowWord(0x12345678ul), 0x5678ul); }
TEST(Util_test, highWord) { EXPECT_EQ(highWord(0x12345678ul), 0x1234ul); }

TEST(Util_test, htons) { EXPECT_EQ(htons(0x1234), 0x3412); }
TEST(Util_test, ntohs) { EXPECT_EQ(ntohs(0x1234), 0x3412); }

TEST(Util_test, htonl) { EXPECT_EQ(htonl(0x12345678ul), 0x78563412ul); }
TEST(Util_test, ntohl) { EXPECT_EQ(ntohl(0x12345678ul), 0x78563412ul); }

TEST(Util_test, hton) {
  EXPECT_EQ(hton(static_cast<uint8_t>(0x12u)), 0x12u);
  EXPECT_EQ(hton(static_cast<int8_t>(0x12)), 0x12);
  EXPECT_EQ(hton(static_cast<uint16_t>(0x1234u)), 0x3412);
  EXPECT_EQ(hton(static_cast<int16_t>(0x1234)), 0x3412);
  EXPECT_EQ(hton(static_cast<uint32_t>(0x12345678ul)), 0x78563412ul);
  EXPECT_EQ(hton(static_cast<int32_t>(0x12345678l)), 0x78563412ul);
}

TEST(Util_test, ntoh) {
  EXPECT_EQ(ntoh(static_cast<uint8_t>(0x12u)), 0x12u);
  EXPECT_EQ(ntoh(static_cast<int8_t>(0x12)), 0x12);
  EXPECT_EQ(ntoh(static_cast<uint16_t>(0x1234u)), 0x3412);
  EXPECT_EQ(ntoh(static_cast<int16_t>(0x1234)), 0x3412);
  EXPECT_EQ(ntoh(static_cast<uint32_t>(0x12345678ul)), 0x78563412ul);
  EXPECT_EQ(ntoh(static_cast<int32_t>(0x12345678l)), 0x78563412ul);
}

// TEST(Util_test, printMillis) {
//   //void printMillis(Stream& stream);
//   ArduinoMock* arduinoMock = arduinoMockInstance();
//   PrintMock printMock;
//   //StreamMock streamMock;
//   SerialMock serial;

//   EXPECT_CALL(*arduinoMock, millis())
//     .WillOnce(Return(UINT32_MAX));
//   EXPECT_CALL(printMock, print('[', DEC)).WillOnce(Return(1));
//   // EXPECT_CALL(printMock, print(UINT32_MAX, DEC)).WillOnce(Return(10));
//   // EXPECT_CALL(printMock, print(']', DEC)).WillOnce(Return(1));
//   printMillis(serial);
//   releaseArduinoMock();
// }
