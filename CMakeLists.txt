cmake_minimum_required(VERSION 3.22)
project(LoRaNodeGarage LANGUAGES CXX)

set(ENABLE_CODECOVERAGE ON)
add_compile_options(-Wno-delete-non-virtual-dtor)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

enable_testing()

add_subdirectory(libraries)
add_subdirectory(tests)
