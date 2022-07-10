# SPDX-License-Identifier: MIT

include_directories("../src")
include_directories("../third-party")

set(TEST_ENCODING_SRC
  "video_encoder_test.cpp"
  "../src/mapping/mapping.cpp"
  "../src/video_encoder/video_encoder.cpp"
)

add_executable(video_encoder_test ${TEST_ENCODING_SRC})
target_link_libraries(video_encoder_test VPL::dispatcher)
add_test(NAME video_encoder_test COMMAND video_encoder_test)
