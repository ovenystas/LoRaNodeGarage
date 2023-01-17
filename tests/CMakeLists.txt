# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
)
FetchContent_MakeAvailable(googletest)

# ---

enable_testing()

include_directories(
  ..
  mocks
)

add_executable(
  Unit_test
  Unit_test.cpp
)
target_link_libraries(
  Unit_test
  GTest::gtest_main
)

add_executable(
  Util_test
  Util_test.cpp
  ../Util.cpp
  mocks/Arduino.cpp
)
target_link_libraries(
  Util_test
  GTest::gtest_main
  GTest::gmock_main
)

add_executable(
  ConfigItem_test
  ConfigItem_test.cpp
  ../Util.cpp
  mocks/Arduino.cpp
)
target_link_libraries(
  ConfigItem_test
  GTest::gtest_main
  GTest::gmock_main
)

add_executable(
  Component_test
  Component_test.cpp
  ../Util.cpp
  mocks/Arduino.cpp
)
target_link_libraries(
  Component_test
  GTest::gtest_main
  GTest::gmock_main
)

add_executable(
  BinarySensor_test
  BinarySensor_test.cpp
  ../BinarySensor.cpp
  ../Util.cpp
  mocks/Arduino.cpp
)
target_link_libraries(
  BinarySensor_test
  GTest::gtest_main
  GTest::gmock_main
)

add_executable(
  Cover_test
  Cover_test.cpp
  ../Cover.cpp
  ../Util.cpp
  mocks/Arduino.cpp
)
target_link_libraries(
  Cover_test
  GTest::gtest_main
  GTest::gmock_main
)

add_executable(
  Sensor_test
  Sensor_test.cpp
  ../Util.cpp
  mocks/Arduino.cpp
)
target_link_libraries(
  Sensor_test
  GTest::gtest_main
  GTest::gmock_main
)

add_executable(
  GarageCover_test
  GarageCover_test.cpp
  ../GarageCover.cpp
  ../Cover.cpp
  ../Util.cpp
  mocks/Arduino.cpp
)
target_link_libraries(
  GarageCover_test
  GTest::gtest_main
  GTest::gmock_main
)

add_executable(
  DistanceSensor_test
  DistanceSensor_test.cpp
  ../DistanceSensor.cpp
  ../Util.cpp
  mocks/Arduino.cpp
)
target_link_libraries(
  DistanceSensor_test
  GTest::gtest_main
  GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(
  Unit_test
)
gtest_discover_tests(
  Util_test
)
gtest_discover_tests(
  ConfigItem_test
)
gtest_discover_tests(
  Component_test
)
gtest_discover_tests(
  BinarySensor_test
)
gtest_discover_tests(
  Cover_test
)
gtest_discover_tests(
  Sensor_test
)
gtest_discover_tests(
  GarageCover_test
)
gtest_discover_tests(
  DistanceSensor_test
)
