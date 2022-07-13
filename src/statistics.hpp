// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <vector>

struct FrameInfo {
  int counter;
  int iframe;
  size_t size;
  long start_time;
  long stop_time;
};

struct Settings {
  std::string codec;
  int gop;
  int fps;
  std::string bitrate;
  std::string mean_bitrate;
  int width;
  int height;
};

struct EncoderMediaFormat {
  int crop_right;
  int color_format;
  std::string mime;
  int stride;
  int intra_refresh_period;
  int color_standard;
  int color_transfer;
  int crop_bottom;
  int crop_left;
  int width;
  int color_range;
  int using_sw_read_often;
  int crop_top;
  int frame_rate;
  int height;
};

struct StatsDataFrame {
  std::string id;
  std::string description;
  std::string test;
  std::string test_definition;
  std::string date;
  std::string encapp_version;
  int proctime;
  int framecount;
  std::string encoded_file;
  std::string source_file;
  Settings settings;
  EncoderMediaFormat encoder_media_format;
  std::vector<FrameInfo> frame_info;
};

class Statistics {
 public:
  Statistics(StatsDataFrame stats_data_frame);

  void write(std::string output_filename);

 private:
  StatsDataFrame stats_data_frame_;
};
