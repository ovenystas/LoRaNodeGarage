#include "../Util.h"

#include <gtest/gtest.h>

using ::testing::Return;

// TODO: Mock millis()
// TEST(Util_test, seconds) {
//   ArduinoMock* arduinoMock = arduinoMockInstance();
//   EXPECT_CALL(*arduinoMock, millis()).WillOnce(Return(1000));
//   EXPECT_EQ(seconds(), 1);
//   releaseArduinoMock();
// }

TEST(Util_test, lowWord) { EXPECT_EQ(lowWord(0x12345678ul), 0x5678ul); }

TEST(Util_test, highWord) { EXPECT_EQ(highWord(0x12345678ul), 0x1234ul); }
