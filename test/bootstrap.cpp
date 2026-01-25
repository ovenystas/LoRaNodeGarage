#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Mock implementations (required for all tests)
#include "mocks/src/Arduino.cpp"
#include "mocks/src/BufferSerial.cpp"
#include "mocks/src/Print.cpp"
#include "mocks/src/Stream.cpp"
#include "mocks/src/EEPROM.cpp"

int main(int argc, char **argv)
{
    ::testing::InitGoogleMock(&argc, argv);

    if (RUN_ALL_TESTS())
    ;

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}
