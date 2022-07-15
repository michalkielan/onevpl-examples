// SPDX-License-Identifier: MIT

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "statistics/statistics.hpp"

#include "nlohmann/json.hpp"

TEST_CASE("When stats saved, read file and check if parameters are the same") {
  StatsDataFrame stats_data_frame;
  std::vector<FrameInfo> frames;

  for (int i = 0; i < 10; i++) {
    FrameInfo frame_info;
    frame_info.counter = i;
    frame_info.iframe = 0;
    frame_info.size = 100 + i;
    frame_info.pts = 1547624774 + i;
    frame_info.start_time = 1547624774 + i;
    frame_info.stop_time = 1547624774 + i + 100;
    frames.emplace_back(std::move(frame_info));
  }

  Settings settings;
  settings.codec = "hevc";
  settings.gop = 1;
  settings.fps = 30;
  settings.bitrate = "2500 kbps";
  settings.mean_bitrate = "2550 kbps";
  settings.width = 720;
  settings.height = 1280;
  EncoderMediaFormat encoder_media_format;

  stats_data_frame.id = "id";
  stats_data_frame.description = "description";
  stats_data_frame.test = "test";
  stats_data_frame.test_definition = "test definition";
  stats_data_frame.date = "2022-01-01";
  stats_data_frame.encapp_version = "v1.6";
  stats_data_frame.proctime = 12345;
  stats_data_frame.framecount = frames.size();
  stats_data_frame.encoded_file = "out.hevc";
  stats_data_frame.source_file = "in.yuv";
  stats_data_frame.settings = settings;
  stats_data_frame.encoder_media_format = encoder_media_format;
  stats_data_frame.frame_info = frames;

  std::stringstream stats_file;

  Statistics statistics{stats_data_frame};
  statistics.write(stats_file);
  std::cout << stats_file.str() << std::endl;

  auto stats_out = nlohmann::json::parse(stats_file);

  CHECK_EQ(stats_out["settings"]["codec"], "hevc");
  CHECK_EQ(stats_out["settings"]["gop"], 1);
  CHECK_EQ(stats_out["settings"]["fps"], 30);
  CHECK_EQ(stats_out["settings"]["bitrate"], "2500 kbps");
  CHECK_EQ(stats_out["settings"]["mean_bitrate"], "2550 kbps");
  CHECK_EQ(stats_out["settings"]["width"], 720);
  CHECK_EQ(stats_out["settings"]["height"], 1280);

  CHECK_EQ(stats_out["id"], "id");
  CHECK_EQ(stats_out["description"], "description");
  CHECK_EQ(stats_out["test"], "test");
  CHECK_EQ(stats_out["testdefinition"], "test definition");
  CHECK_EQ(stats_out["date"], "2022-01-01");
  CHECK_EQ(stats_out["encapp_version"], "v1.6");
  CHECK_EQ(stats_out["proctime"], 12345);
  CHECK_EQ(stats_out["framecount"], frames.size());
  CHECK_EQ(stats_out["encodedfile"], "out.hevc");
  CHECK_EQ(stats_out["sourcefile"], "in.yuv");

  int i = 0;
  for (auto& frame_info : stats_out["frame_info"]) {
    CHECK_EQ(frame_info["counter"], i);
    CHECK_EQ(frame_info["iframe"], 0);
    CHECK_EQ(frame_info["size"], 100 + i);
    CHECK_EQ(frame_info["iframe"], 0);
    CHECK_EQ(frame_info["proctime"], 100);
    CHECK_EQ(frame_info["pts"], 1547624774 + i);
    CHECK_EQ(frame_info["start_time"], 1547624774 + i);
    CHECK_EQ(frame_info["stop_time"], 1547624774 + i + 100);
    ++i;
  }
}

