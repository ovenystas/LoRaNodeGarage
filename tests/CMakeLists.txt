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
