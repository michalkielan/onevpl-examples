# SPDX-License-Identifier: MIT

include_directories("../src")
include_directories("../third-party")

set(TEST_ENCODING_SRC
  "test_encoding.cpp"
  "../src/mapping.cpp"
  "../src/video_encoder.cpp"
)

add_executable(test_encoding ${TEST_ENCODING_SRC})
target_link_libraries(test_encoding VPL::dispatcher)
add_test(NAME test_encoding COMMAND test_encoding)
