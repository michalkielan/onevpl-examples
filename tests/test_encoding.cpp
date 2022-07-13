// SPDX-License-Identifier: MIT

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "vpl/preview/vpl.hpp"

#include "utils.hpp"
#include "video_encoder.hpp"

namespace vpl = oneapi::vpl;

TEST_CASE("Init CPU encoder") {
  const auto input_fourcc = vpl::color_format_fourcc::i420;
  vpl::default_selector impl_sel{
      {vpl::dprops::impl(vpl::implementation_type::sw),
       vpl::dprops::api_version(2, 5),
       vpl::dprops::encoder({vpl::dprops::codec_id(vpl::codec_format_fourcc::hevc)})}};
  std::ifstream input_file{"../res/cars_320x240.i420", std::ios_base::in | std::ios_base::binary};
  vpl::raw_frame_file_reader frame_file_reader{320, 240, input_fourcc, input_file};

  VideoEncoder video_encoder{impl_sel, &frame_file_reader};

  vpl::frame_info info{};
  info.set_frame_rate({30, 1});
  info.set_frame_size({ALIGN16(320), ALIGN16(240)});
  info.set_FourCC(input_fourcc);
  info.set_ChromaFormat(vpl::chroma_format_idc::yuv420);
  info.set_ROI({{0, 0}, {320, 240}});
  info.set_PicStruct(vpl::pic_struct::progressive);
  video_encoder.init(
      std::move(info), vpl::codec_format_fourcc::hevc, vpl::rate_control_method::cqp);
}
