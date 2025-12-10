cmake_minimum_required(VERSION 3.14)
project(LargeVolumeTrading)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)
include_directories(src/market)
include_directories(src/order)
include_directories(src/strategy)

file(GLOB SOURCES
    src/market/*.cpp
    src/order/*.cpp
    src/strategy/*.cpp
)

add_executable(LargeVolumeTrading main.cpp ${SOURCES})

# Example executables
add_executable(vwap_example examples/vwap_example.cpp ${SOURCES})
add_executable(optimal_speed_example examples/optimal_speed_example.cpp ${SOURCES})
add_executable(almgren_kriss_example examples/almgren_kriss_example.cpp ${SOURCES})

# GoogleTest Integration
include(FetchContent)
cmake_policy(SET CMP0135 NEW)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
file(GLOB TEST_SOURCES tests/*.cpp)
add_executable(LargeVolumeTradingTests ${TEST_SOURCES} ${SOURCES})
target_link_libraries(LargeVolumeTradingTests gtest gtest_main)
add_test(NAME LargeVolumeTradingTests COMMAND LargeVolumeTradingTests)
