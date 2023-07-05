# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

add_compile_definitions(TESTING="true")

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

add_executable(Unit_test EXCLUDE_FROM_ALL Unit_test.cpp)
target_link_libraries(Unit_test GTest::gtest_main)
gtest_discover_tests(Unit_test)

add_executable(Util_test EXCLUDE_FROM_ALL Util_test.cpp ../Util.cpp
                                          mocks/Arduino.cpp)
target_link_libraries(Util_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(Util_test)

add_executable(ConfigItem_test EXCLUDE_FROM_ALL ConfigItem_test.cpp ../Util.cpp
                                                mocks/Arduino.cpp)
target_link_libraries(ConfigItem_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(ConfigItem_test)

add_executable(BaseComponent_test EXCLUDE_FROM_ALL
               BaseComponent_test.cpp ../Util.cpp mocks/Arduino.cpp)
target_link_libraries(BaseComponent_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(BaseComponent_test)

add_executable(
  BinarySensor_test EXCLUDE_FROM_ALL BinarySensor_test.cpp ../BinarySensor.cpp
                                     ../Util.cpp mocks/Arduino.cpp)
target_link_libraries(BinarySensor_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(BinarySensor_test)

add_executable(Cover_test EXCLUDE_FROM_ALL Cover_test.cpp ../Cover.cpp
                                           ../Util.cpp mocks/Arduino.cpp)
target_link_libraries(Cover_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(Cover_test)

add_executable(Sensor_test EXCLUDE_FROM_ALL Sensor_test.cpp ../Util.cpp
                                            mocks/Arduino.cpp)
target_link_libraries(Sensor_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(Sensor_test)

add_executable(
  GarageCover_test EXCLUDE_FROM_ALL GarageCover_test.cpp ../GarageCover.cpp
                                    ../Cover.cpp ../Util.cpp mocks/Arduino.cpp)
target_link_libraries(GarageCover_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(GarageCover_test)

add_executable(
  DistanceSensor_test EXCLUDE_FROM_ALL
  DistanceSensor_test.cpp ../DistanceSensor.cpp ../Util.cpp mocks/Arduino.cpp)
target_link_libraries(DistanceSensor_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(DistanceSensor_test)

add_executable(
  HeightSensor_test EXCLUDE_FROM_ALL HeightSensor_test.cpp ../HeightSensor.cpp
                                     ../Util.cpp mocks/Arduino.cpp)
target_link_libraries(HeightSensor_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(HeightSensor_test)

add_executable(
  HumiditySensor_test EXCLUDE_FROM_ALL
  HumiditySensor_test.cpp ../HumiditySensor.cpp ../Util.cpp mocks/Arduino.cpp)
target_link_libraries(HumiditySensor_test GTest::gtest_main GTest::gmock_main)
gtest_discover_tests(HumiditySensor_test)

add_executable(
  PresenceBinarySensor_test EXCLUDE_FROM_ALL
  PresenceBinarySensor_test.cpp ../PresenceBinarySensor.cpp ../Util.cpp
  ../BinarySensor.cpp mocks/Arduino.cpp)
target_link_libraries(PresenceBinarySensor_test GTest::gtest_main
                      GTest::gmock_main)
gtest_discover_tests(PresenceBinarySensor_test)

add_executable(
  TemperatureSensor_test EXCLUDE_FROM_ALL
  TemperatureSensor_test.cpp ../TemperatureSensor.cpp ../Util.cpp
  mocks/Arduino.cpp)
target_link_libraries(TemperatureSensor_test GTest::gtest_main
                      GTest::gmock_main)
gtest_discover_tests(TemperatureSensor_test)

add_custom_target(
  test_all
  COMMENT "Target to build all unit tests"
  DEPENDS Unit_test
          Util_test
          ConfigItem_test
          BaseComponent_test
          BinarySensor_test
          Cover_test
          Sensor_test
          GarageCover_test
          DistanceSensor_test
          HeightSensor_test
          HumiditySensor_test
          PresenceBinarySensor_test
          TemperatureSensor_test)
