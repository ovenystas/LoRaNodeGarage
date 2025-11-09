# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

add_compile_definitions(TESTING="true")

include(CTest)

add_compile_options(
  -g
  -O0
  --coverage
  -pedantic
  -Wall
  -Wextra
  -Wcast-align
  -Wcast-qual
  -Wdisabled-optimization
  -Wformat=2
  -Winit-self
  -Wlogical-op
  -Wmissing-include-dirs
  -Wnoexcept
  -Wold-style-cast
  -Woverloaded-virtual
  -Wredundant-decls
  -Wshadow
  -Wsign-promo
  -Wstrict-null-sentinel
  -Wstrict-overflow=5
  -Werror
  -Wno-unused)
add_link_options(--coverage)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
)
FetchContent_MakeAvailable(googletest)

# ---

include(GoogleTest)
include(CodeCoverage)

include_directories(.. mocks)

add_executable(
  AirTime_test EXCLUDE_FROM_ALL
  AirTime_test.cpp ../AirTime.cpp ../Util.cpp mocks/Arduino.cpp
  mocks/BufferSerial.cpp mocks/Print.cpp)
target_link_libraries(AirTime_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(AirTime_test)

add_executable(
  BaseComponent_test EXCLUDE_FROM_ALL
  BaseComponent_test.cpp ../Util.cpp mocks/Arduino.cpp mocks/BufferSerial.cpp
  mocks/BufferSerial.cpp mocks/Print.cpp)
target_link_libraries(BaseComponent_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(BaseComponent_test)

add_executable(
  BinarySensor_test EXCLUDE_FROM_ALL
  BinarySensor_test.cpp ../BinarySensor.cpp ../Util.cpp mocks/Arduino.cpp
  mocks/Print.cpp mocks/BufferSerial.cpp)
target_link_libraries(BinarySensor_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(BinarySensor_test)

add_executable(
  ConfigItem_test EXCLUDE_FROM_ALL
  ConfigItem_test.cpp
  ../Util.cpp
  mocks/Arduino.cpp
  mocks/BufferSerial.cpp
  mocks/Arduino.cpp
  mocks/Print.cpp
  mocks/EEPROM.cpp)
target_link_libraries(ConfigItem_test GTest::gtest_main GTest::gmock_main
                      crc_crc8)
target_include_directories(ConfigItem_test PRIVATE ../libraries/CRC/src)
gtest_discover_tests(ConfigItem_test)

add_executable(
  Cover_test EXCLUDE_FROM_ALL
  Cover_test.cpp
  ../Cover.cpp
  ../Util.cpp
  mocks/Arduino.cpp
  mocks/Print.cpp
  mocks/Print.cpp
  mocks/BufferSerial.cpp)
target_link_libraries(Cover_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(Cover_test)

add_executable(
  DistanceSensor_test EXCLUDE_FROM_ALL
  DistanceSensor_test.cpp
  ../DistanceSensor.cpp
  ../Util.cpp
  mocks/Arduino.cpp
  mocks/Print.cpp
  mocks/BufferSerial.cpp
  mocks/EEPROM.cpp)
target_link_libraries(DistanceSensor_test GTest::gtest_main GTest::gmock_main
                      crc_crc8)
target_include_directories(DistanceSensor_test PRIVATE ../libraries/CRC/src)
gtest_discover_tests(DistanceSensor_test)

add_executable(
  GarageCover_test EXCLUDE_FROM_ALL
  GarageCover_test.cpp
  ../GarageCover.cpp
  ../Cover.cpp
  ../Util.cpp
  mocks/Arduino.cpp
  mocks/Print.cpp
  mocks/BufferSerial.cpp
  mocks/EEPROM.cpp)
target_link_libraries(GarageCover_test GTest::gtest_main GTest::gmock_main
                      crc_crc8)
target_include_directories(GarageCover_test PRIVATE ../libraries/CRC/src)
gtest_discover_tests(GarageCover_test)

add_executable(
  HeightSensor_test EXCLUDE_FROM_ALL
  HeightSensor_test.cpp
  ../HeightSensor.cpp
  ../Util.cpp
  mocks/Arduino.cpp
  mocks/Print.cpp
  mocks/BufferSerial.cpp
  mocks/EEPROM.cpp)
target_link_libraries(HeightSensor_test GTest::gtest_main GTest::gmock_main
                      crc_crc8)
target_include_directories(HeightSensor_test PRIVATE ../libraries/CRC/src)
gtest_discover_tests(HeightSensor_test)

add_executable(
  HumiditySensor_test EXCLUDE_FROM_ALL
  HumiditySensor_test.cpp
  ../HumiditySensor.cpp
  ../Util.cpp
  mocks/Arduino.cpp
  mocks/Print.cpp
  mocks/BufferSerial.cpp
  mocks/EEPROM.cpp)
target_link_libraries(HumiditySensor_test GTest::gtest_main GTest::gmock_main
                      crc_crc8)
target_include_directories(HumiditySensor_test PRIVATE ../libraries/CRC/src)
gtest_discover_tests(HumiditySensor_test)

add_executable(
  LoRaHandler_test EXCLUDE_FROM_ALL
  LoRaHandler_test.cpp
  ../LoRaHandler.cpp
  ../Util.cpp
  ../AirTime.cpp
  mocks/Arduino.cpp
  mocks/BufferSerial.cpp
  mocks/Print.cpp)
target_link_libraries(LoRaHandler_test GTest::gtest_main GTest::gmock_main
                      crypto_ctr_aes128)
target_include_directories(LoRaHandler_test SYSTEM
                           PRIVATE ../libraries/arduinolibs/libraries/Crypto)
# target_compile_options(LoRaHandler_test PRIVATE -Wold-style-cast)
gtest_discover_tests(LoRaHandler_test)

add_executable(Node_test Node_test.cpp ../Node.cpp mocks/Print.cpp
                         mocks/BufferSerial.cpp)
target_link_libraries(Node_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(Node_test)

add_executable(
  PresenceBinarySensor_test EXCLUDE_FROM_ALL
  PresenceBinarySensor_test.cpp
  ../PresenceBinarySensor.cpp
  ../Util.cpp
  ../BinarySensor.cpp
  mocks/Arduino.cpp
  mocks/Print.cpp
  mocks/BufferSerial.cpp
  mocks/EEPROM.cpp)
target_link_libraries(PresenceBinarySensor_test GTest::gtest_main
                      GTest::gmock_main crc_crc8)
target_include_directories(PresenceBinarySensor_test
                           PRIVATE ../libraries/CRC/src)
gtest_discover_tests(PresenceBinarySensor_test)

add_executable(RingBuffer_test EXCLUDE_FROM_ALL RingBuffer_test.cpp)
target_link_libraries(RingBuffer_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(RingBuffer_test)

add_executable(
  Sensor_test EXCLUDE_FROM_ALL Sensor_test.cpp ../Util.cpp mocks/Arduino.cpp
                               mocks/Print.cpp mocks/BufferSerial.cpp)
target_link_libraries(Sensor_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(Sensor_test)

add_executable(
  TemperatureSensor_test EXCLUDE_FROM_ALL
  TemperatureSensor_test.cpp
  ../TemperatureSensor.cpp
  ../Util.cpp
  mocks/Arduino.cpp
  mocks/Print.cpp
  mocks/BufferSerial.cpp
  mocks/EEPROM.cpp)
target_link_libraries(TemperatureSensor_test GTest::gtest_main
                      GTest::gmock_main crc_crc8)
target_include_directories(TemperatureSensor_test PRIVATE ../libraries/CRC/src)
gtest_discover_tests(TemperatureSensor_test)

add_executable(
  Types_test EXCLUDE_FROM_ALL Types_test.cpp ../Util.cpp mocks/Arduino.cpp
                              mocks/BufferSerial.cpp mocks/Print.cpp)
target_link_libraries(Types_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(Types_test)

add_executable(
  Unit_test EXCLUDE_FROM_ALL Unit_test.cpp mocks/Arduino.cpp mocks/Print.cpp
                             mocks/BufferSerial.cpp)
target_link_libraries(Unit_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(Unit_test)

add_executable(
  Util_test EXCLUDE_FROM_ALL Util_test.cpp ../Util.cpp mocks/Arduino.cpp
                             mocks/Print.cpp mocks/BufferSerial.cpp)
target_link_libraries(Util_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(Util_test)

add_executable(
  ValueItem_test EXCLUDE_FROM_ALL ValueItem_test.cpp mocks/Arduino.cpp
                                  mocks/Print.cpp mocks/BufferSerial.cpp)
target_link_libraries(ValueItem_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(ValueItem_test)

add_custom_target(
  test_all
  COMMENT "Target to build all unit tests"
  DEPENDS AirTime_test
          BaseComponent_test
          BinarySensor_test
          ConfigItem_test
          Cover_test
          DistanceSensor_test
          GarageCover_test
          HeightSensor_test
          HumiditySensor_test
          LoRaHandler_test
          Node_test
          PresenceBinarySensor_test
          RingBuffer_test
          Sensor_test
          TemperatureSensor_test
          Types_test
          Unit_test
          Util_test
          ValueItem_test)
