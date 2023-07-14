#include "../Util.h"

#include <gtest/gtest.h>

#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"

using ::testing::Return;

char strBuf[256];

void bufSerReadStr(Stream* pSerial) {
  size_t i = 0;
  while (pSerial->available()) {
    int c = pSerial->read();
    if (c < 0) {
      break;
    }
    strBuf[i++] = static_cast<char>(c);
  }
  strBuf[i] = '\0';
}

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

TEST(Util_test, printUptime_millis_0) {
  BufferSerial bufSer = BufferSerial(32);
  ArduinoMock* pArduinoMock = arduinoMockInstance();
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(0));

  printUptime(bufSer);

  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "[0:00:00.000] ");
  releaseArduinoMock();
}

TEST(Util_test, printUptime_millis_1000_59_59_999) {
  BufferSerial bufSer = BufferSerial(32);
  ArduinoMock* pArduinoMock = arduinoMockInstance();
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(1000L * 3600000L + 59L * 60000L + 59L * 1000L + 999L));

  printUptime(bufSer);

  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "[1000:59:59.999] ");
  releaseArduinoMock();
}

TEST(Util_test, printUptime_millis_UINT32_MAX) {
  BufferSerial bufSer = BufferSerial(32);
  ArduinoMock* pArduinoMock = arduinoMockInstance();
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(UINT32_MAX));

  printUptime(bufSer);

  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "[1193:02:47.295] ");
  releaseArduinoMock();
}

TEST(Util_test, printMillis_0) {
  BufferSerial bufSer = BufferSerial(32);
  ArduinoMock* pArduinoMock = arduinoMockInstance();
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(0));

  printMillis(bufSer);

  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "[0] ");
  releaseArduinoMock();
}

TEST(Util_test, printMillis_UINT32_MAX) {
  BufferSerial bufSer = BufferSerial(32);
  ArduinoMock* pArduinoMock = arduinoMockInstance();
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(UINT32_MAX));

  printMillis(bufSer);

  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "[4294967295] ");
  releaseArduinoMock();
}

TEST(Util_test, printVersion_0_0_0) {
  BufferSerial bufSer = BufferSerial(32);

  printVersion(bufSer, 0, 0, 0);

  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "0.0.0");
}

TEST(Util_test, printVersion_255_255_255) {
  BufferSerial bufSer = BufferSerial(32);

  printVersion(bufSer, UINT8_MAX, UINT8_MAX, UINT8_MAX);

  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "255.255.255");
}

TEST(Util_test, printHex_uint8) {
  BufferSerial bufSer = BufferSerial(32);

  printHex(bufSer, static_cast<uint8_t>(0));
  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "00");

  printHex(bufSer, static_cast<uint8_t>(0), true);
  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "0x00");

  printHex(bufSer, static_cast<uint8_t>(UINT8_MAX));
  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "FF");

  printHex(bufSer, static_cast<uint8_t>(UINT8_MAX), true);
  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "0xFF");
}

TEST(Util_test, printHex_uint16) {
  BufferSerial bufSer = BufferSerial(32);

  printHex(bufSer, static_cast<uint16_t>(0));
  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "0000");

  printHex(bufSer, static_cast<uint16_t>(0), true);
  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "0x0000");

  printHex(bufSer, static_cast<uint16_t>(UINT16_MAX));
  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "FFFF");

  printHex(bufSer, static_cast<uint16_t>(UINT16_MAX), true);
  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "0xFFFF");
}

TEST(Util_test, printHex_uint32) {
  BufferSerial bufSer = BufferSerial(32);

  printHex(bufSer, static_cast<uint32_t>(0));
  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "00000000");

  printHex(bufSer, static_cast<uint32_t>(0), true);
  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "0x00000000");

  printHex(bufSer, static_cast<uint32_t>(UINT32_MAX));
  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "FFFFFFFF");

  printHex(bufSer, static_cast<uint32_t>(UINT32_MAX), true);
  bufSerReadStr(&bufSer);
  EXPECT_STREQ(strBuf, "0xFFFFFFFF");
}
