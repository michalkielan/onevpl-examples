// SPDX-License-Identifier: MIT

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "vpl/preview/vpl.hpp"

#include "utils.hpp"
#include "video_encoder/video_encoder.hpp"

namespace vpl = oneapi::vpl;

TEST_CASE("When init CPU encoder, parameters should be correct") {
  uint16_t width;
  uint16_t height;
  int frame_rate;
  std::string input_filename;
  SUBCASE("cars_320x240.i420") {
    width = 240;
    height = 320;
    frame_rate = 30;
    input_filename = "../res/cars_320x240.i420";
  }

  CAPTURE(width);
  CAPTURE(height);
  CAPTURE(frame_rate);
  CAPTURE(input_filename);
  const auto input_fourcc = vpl::color_format_fourcc::i420;
  const auto pic_struct = vpl::pic_struct::progressive;
  const auto chroma_format = vpl::chroma_format_idc::yuv420;

  vpl::default_selector impl_sel{
      {vpl::dprops::impl(vpl::implementation_type::sw),
       vpl::dprops::api_version(2, 5),
       vpl::dprops::encoder({vpl::dprops::codec_id(vpl::codec_format_fourcc::hevc)})}};
  std::ifstream input_file{input_filename, std::ios_base::in | std::ios_base::binary};
  vpl::raw_frame_file_reader frame_file_reader{width, height, input_fourcc, input_file};

  VideoEncoder video_encoder{impl_sel, &frame_file_reader};

  try {
    vpl::frame_info info{};
    info.set_frame_rate({frame_rate, 1});
    info.set_frame_size({ALIGN16(width), ALIGN16(height)});
    info.set_FourCC(input_fourcc);
    info.set_ChromaFormat(chroma_format);
    info.set_ROI({{0, 0}, {width, height}});
    info.set_PicStruct(pic_struct);
    video_encoder.init(
        std::move(info), vpl::codec_format_fourcc::hevc, vpl::rate_control_method::cqp);
  } catch (vpl::base_exception const& exc) {
    CHECK_MESSAGE(false, "Encoder init failed: ", std::string(exc.what()));
  }
}

TEST_CASE("When init encoder with external buffers, not crash") {
  uint16_t width;
  uint16_t height;
  int frame_rate;
  std::string input_filename;
  SUBCASE("cars_320x240.i420") {
    width = 240;
    height = 320;
    frame_rate = 30;
    input_filename = "../res/cars_320x240.i420";
  }

  CAPTURE(width);
  CAPTURE(height);
  CAPTURE(frame_rate);
  CAPTURE(input_filename);
  const auto input_fourcc = vpl::color_format_fourcc::i420;
  const auto pic_struct = vpl::pic_struct::progressive;
  const auto chroma_format = vpl::chroma_format_idc::yuv420;

  vpl::default_selector impl_sel{
      {vpl::dprops::impl(vpl::implementation_type::sw),
       vpl::dprops::api_version(2, 5),
       vpl::dprops::encoder({vpl::dprops::codec_id(vpl::codec_format_fourcc::hevc)})}};
  std::ifstream input_file{input_filename, std::ios_base::in | std::ios_base::binary};
  vpl::raw_frame_file_reader frame_file_reader{width, height, input_fourcc, input_file};

  VideoEncoder video_encoder{impl_sel, &frame_file_reader};

  vpl::encoder_init_list encoder_init_list;
  auto roi_config = std::make_unique<vpl::ExtEncoderROI>();
  roi_config->get_ref().NumROI = 1;
  roi_config->get_ref().ROIMode = MFX_ROI_MODE_QP_DELTA;
  roi_config->get_ref().ROI[0].Left = 160;
  roi_config->get_ref().ROI[0].Right = 640;
  roi_config->get_ref().ROI[0].Top = 0;
  roi_config->get_ref().ROI[0].Bottom = 320;
  // https://spec.oneapi.io/versions/latest/elements/oneVPL/source/API_ref/VPL_structs_encode.html?#_CPPv4N16mfxExtEncoderROI7DeltaQPE
  roi_config->get_ref().ROI[0].DeltaQP = -51;
  encoder_init_list.add_buffer(roi_config.get());

  try {
    vpl::frame_info info{};
    info.set_frame_rate({frame_rate, 1});
    info.set_frame_size({ALIGN16(width), ALIGN16(height)});
    info.set_FourCC(input_fourcc);
    info.set_ChromaFormat(chroma_format);
    info.set_ROI({{0, 0}, {width, height}});
    info.set_PicStruct(pic_struct);
    video_encoder.init(std::move(info),
                       vpl::codec_format_fourcc::hevc,
                       vpl::rate_control_method::cqp,
                       encoder_init_list);
  } catch (vpl::base_exception const& exc) {
    CHECK_MESSAGE(false, "Encoder init failed: ", std::string(exc.what()));
  }
}
