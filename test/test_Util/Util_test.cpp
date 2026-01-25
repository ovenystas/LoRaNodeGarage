#include "Util.h"

#include <gtest/gtest.h>

#include "Arduino.h"
#include "BufferSerial.h"

// Include source implementation
#include "../../src/Util.cpp"

using ::testing::Return;

char strBuf[256];

void bufSerReadStr() {
  size_t i = 0;
  while (Serial.available()) {
    int c = Serial.read();
    if (c < 0) {
      break;
    }
    strBuf[i++] = static_cast<char>(c);
  }
  strBuf[i] = '\0';
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
  ArduinoMock* pArduinoMock = arduinoMockInstance();
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(0));

  printUptime(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, "[0:00:00.000] ");
  releaseArduinoMock();
}

TEST(Util_test, printUptime_millis_1000_59_59_999) {
  ArduinoMock* pArduinoMock = arduinoMockInstance();
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(1000UL * 3600000UL + 59UL * 60000UL + 59UL * 1000UL + 999UL));

  printUptime(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, "[1000:59:59.999] ");
  releaseArduinoMock();
}

TEST(Util_test, printUptime_millis_UINT32_MAX) {
  ArduinoMock* pArduinoMock = arduinoMockInstance();
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(UINT32_MAX));

  printUptime(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, "[1193:02:47.295] ");
  releaseArduinoMock();
}

TEST(Util_test, printMillis_0) {
  ArduinoMock* pArduinoMock = arduinoMockInstance();
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(0));

  printMillis(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, "\n[0] ");
  releaseArduinoMock();
}

TEST(Util_test, printMillis_UINT32_MAX) {
  ArduinoMock* pArduinoMock = arduinoMockInstance();
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(UINT32_MAX));

  printMillis(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, "\n[4294967295] ");
  releaseArduinoMock();
}

TEST(Util_test, printVersion_0_0_0) {
  printVersion(Serial, 0, 0, 0);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, "0.0.0");
}

TEST(Util_test, printVersion_255_255_255) {
  printVersion(Serial, UINT8_MAX, UINT8_MAX, UINT8_MAX);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, "255.255.255");
}

TEST(Util_test, printHex_uint8) {
  printHex(Serial, static_cast<uint8_t>(0));
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "00");

  printHex(Serial, static_cast<uint8_t>(0), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "0x00");

  printHex(Serial, static_cast<uint8_t>(UINT8_MAX));
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "FF");

  printHex(Serial, static_cast<uint8_t>(UINT8_MAX), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "0xFF");
}

TEST(Util_test, printHex_uint16) {
  printHex(Serial, static_cast<uint16_t>(0));
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "0000");

  printHex(Serial, static_cast<uint16_t>(0), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "0x0000");

  printHex(Serial, static_cast<uint16_t>(UINT16_MAX));
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "FFFF");

  printHex(Serial, static_cast<uint16_t>(UINT16_MAX), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "0xFFFF");
}

TEST(Util_test, printHex_uint32) {
  printHex(Serial, static_cast<uint32_t>(0));
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "00000000");

  printHex(Serial, static_cast<uint32_t>(0), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "0x00000000");

  printHex(Serial, static_cast<uint32_t>(UINT32_MAX));
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "FFFFFFFF");

  printHex(Serial, static_cast<uint32_t>(UINT32_MAX), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "0xFFFFFFFF");
}

TEST(Util_test, printOct) {
  printOct(Serial, static_cast<uint8_t>(0), false);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "000");

  printOct(Serial, static_cast<uint8_t>(0), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "o000");

  printOct(Serial, static_cast<uint8_t>(0x5A), false);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "132");

  printOct(Serial, static_cast<uint8_t>(0x5A), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "o132");

  printOct(Serial, static_cast<uint8_t>(UINT8_MAX), false);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "377");

  printOct(Serial, static_cast<uint8_t>(UINT8_MAX), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "o377");
}

TEST(Util_test, printBin) {
  printBin(Serial, static_cast<uint8_t>(0), false);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "00000000");

  printBin(Serial, static_cast<uint8_t>(0), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "b00000000");

  printBin(Serial, static_cast<uint8_t>(0x5A), false);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "01011010");

  printBin(Serial, static_cast<uint8_t>(0x5A), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "b01011010");

  printBin(Serial, static_cast<uint8_t>(UINT8_MAX), false);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "11111111");

  printBin(Serial, static_cast<uint8_t>(UINT8_MAX), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "b11111111");
}

TEST(Util_test, printDec) {
  printDec(Serial, static_cast<uint8_t>(0), false);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "0");

  printDec(Serial, static_cast<uint8_t>(0), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "d0");

  printDec(Serial, static_cast<uint8_t>(0x5A), false);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "90");

  printDec(Serial, static_cast<uint8_t>(0x5A), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "d90");

  printDec(Serial, static_cast<uint8_t>(UINT8_MAX), false);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "255");

  printDec(Serial, static_cast<uint8_t>(UINT8_MAX), true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf, "d255");
}

TEST(Util_test, printArray) {
  uint8_t buf[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99,
                   0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

  printArray(Serial, buf, sizeof(buf), HEX, true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf,
               "0x00 0x11 0x22 0x33 0x44 0x55 0x66 0x77 "
               "0x88 0x99 0xAA 0xBB 0xCC 0xDD 0xEE 0xFF");

  printArray(Serial, buf, sizeof(buf), DEC, false);
  bufSerReadStr();
  EXPECT_STREQ(strBuf,
               "0 17 34 51 68 85 102 119 "
               "136 153 170 187 204 221 238 255");
  
  printArray(Serial, buf, sizeof(buf), OCT, true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf,
               "o000 o021 o042 o063 o104 o125 o146 o167 "
               "o210 o231 o252 o273 o314 o335 o356 o377");
  
  printArray(Serial, buf, sizeof(buf), BIN, true);
  bufSerReadStr();
  EXPECT_STREQ(strBuf,
               "b00000000 b00010001 b00100010 b00110011 "
               "b01000100 b01010101 b01100110 b01110111 "
               "b10001000 b10011001 b10101010 b10111011 "
               "b11001100 b11011101 b11101110 b11111111");
}
