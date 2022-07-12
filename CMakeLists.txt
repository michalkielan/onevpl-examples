# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10)

project(encodeapp)
set(TARGET encodeapp)

find_package(VPL REQUIRED)

include_directories("third-party")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra -Wpedantic")
set(CMAKE_BUILD_TYPE Debug)

set(SOURCES
  "src/main.cpp"
  "src/mapping.cpp"
  "src/statistics.cpp"
  "src/video_encoder.cpp"
)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET} VPL::dispatcher)
