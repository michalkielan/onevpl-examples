# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10)

project(encodeapp)
set(TARGET encodeapp)

enable_testing()

set(BUILD_TESTS ON CACHE BOOL "Build tests")

find_package(VPL REQUIRED)

include_directories("third-party")
include_directories("src")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra -Wpedantic")
set(CMAKE_BUILD_TYPE Debug)

set(SOURCES
  "src/encodeapp/main.cpp"
  "src/mapping/mapping.cpp"
  "src/statistics/statistics.cpp"
  "src/video_encoder/video_encoder.cpp"
)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET} VPL::dispatcher)

if(BUILD_TESTS)
  add_subdirectory("tests")
endif()
