# SPDX-License-Identifier: MIT

include_directories("../src")
include_directories("../third-party")

set(VIDEO_ENCODER_TEST_SRC
  "video_encoder_test.cpp"
  "../src/mapping/mapping.cpp"
  "../src/video_encoder/video_encoder.cpp"
)
add_executable(video_encoder_test ${VIDEO_ENCODER_TEST_SRC})
target_link_libraries(video_encoder_test VPL::dispatcher)
add_test(NAME video_encoder_test COMMAND video_encoder_test)


set(STATISTICS_TEST_SRC
  "statistics_test.cpp"
  "../src/statistics/statistics.cpp"
)
add_executable(statistics_test ${STATISTICS_TEST_SRC})
add_test(NAME statistics_test COMMAND statistics_test)
