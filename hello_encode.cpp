//==============================================================================
// Copyright Intel Corporation
//
// SPDX-License-Identifier: MIT
//==============================================================================

///
/// A minimal oneAPI Video Processing Library (oneVPL) encode application,
/// using 2.x API with internal memory management
///
/// @file

#include <fstream>
#include <iostream>
#include <map>

#include "cxxopts.hpp"
#include "vpl/preview/vpl.hpp"

#define ALIGN16(value) (((value + 15) >> 4) << 4)

constexpr const int kWait100Ms = 100;
constexpr const bool kUseVideoMemory = false;

namespace vpl = oneapi::vpl;

static void writeEncodedStream(std::shared_ptr<vpl::bitstream_as_dst> bits,
                               std::ofstream* fileStream) {
  auto [ptr, len] = bits->get_valid_data();
  fileStream->write(reinterpret_cast<char*>(ptr), len);
  bits->set_DataLength(0);
  return;
}

const std::map<std::string, vpl::codec_format_fourcc> codec_formats{
    {"avc", vpl::codec_format_fourcc::avc},
    {"hevc", vpl::codec_format_fourcc::hevc},
    {"mpeg2", vpl::codec_format_fourcc::mpeg2},
    {"vc1", vpl::codec_format_fourcc::vc1},
    {"capture", vpl::codec_format_fourcc::capture},
    {"vp9", vpl::codec_format_fourcc::vp9},
    {"av1", vpl::codec_format_fourcc::avc},
};

const std::map<std::string, vpl::chroma_format_idc> chroma_formats{
    {"monochrome", vpl::chroma_format_idc::monochrome},
    {"yuv420", vpl::chroma_format_idc::yuv420},
    {"yuv422", vpl::chroma_format_idc::yuv422},
    {"yuv444", vpl::chroma_format_idc::yuv444},
    {"yuv400", vpl::chroma_format_idc::yuv400},
    {"yuv411", vpl::chroma_format_idc::yuv411},
    {"yuv422h", vpl::chroma_format_idc::yuv422h},
    {"yuv422v", vpl::chroma_format_idc::yuv422v},
};

const std::map<std::string, vpl::color_format_fourcc> color_formats{
    {"nv12", vpl::color_format_fourcc::nv12},
    {"yv12", vpl::color_format_fourcc::yv12},
    {"nv16", vpl::color_format_fourcc::nv16},
    {"yuy2", vpl::color_format_fourcc::yuy2},
    {"rgb465", vpl::color_format_fourcc::rgb465},
    {"rgbp", vpl::color_format_fourcc::rgbp},
    {"rgb3", vpl::color_format_fourcc::rgb3},
    {"bgra", vpl::color_format_fourcc::bgra},
    {"p8", vpl::color_format_fourcc::p8},
    {"p8_texture", vpl::color_format_fourcc::p8_texture},
    {"p010", vpl::color_format_fourcc::p010},
    {"p016", vpl::color_format_fourcc::p016},
    {"p210", vpl::color_format_fourcc::p210},
    {"bgr4", vpl::color_format_fourcc::bgr4},
    {"a2rgb10", vpl::color_format_fourcc::a2rgb10},
    {"argb16", vpl::color_format_fourcc::argb16},
    {"abgr16", vpl::color_format_fourcc::abgr16},
    {"r16", vpl::color_format_fourcc::r16},
    {"ayuv", vpl::color_format_fourcc::ayuv},
    {"ayuv_rgb4", vpl::color_format_fourcc::ayuv_rgb4},
    {"uyvy", vpl::color_format_fourcc::uyvy},
    {"y210", vpl::color_format_fourcc::y210},
    {"y410", vpl::color_format_fourcc::y410},
    {"y216", vpl::color_format_fourcc::y216},
    {"y416", vpl::color_format_fourcc::y416},
    {"nv21", vpl::color_format_fourcc::nv21},
    {"i420", vpl::color_format_fourcc::i420},
    {"i010", vpl::color_format_fourcc::i010},
    {"i210", vpl::color_format_fourcc::i210},
    {"i422", vpl::color_format_fourcc::i422},
    {"bgrp", vpl::color_format_fourcc::bgrp},
};

int main(int argc, char** argv) {
  cxxopts::Options options{"Encode app", "oneVPL encode application."};
  options.add_options(
      "hello_encode",
      {{"i,input", "Input file", cxxopts::value<std::string>()},
       {"o,output", "Output file", cxxopts::value<std::string>()},
       {"h,height", "Height", cxxopts::value<int>()->default_value("0")},
       {"w,width", "Width", cxxopts::value<int>()->default_value("0")},
       {"r,rate", "Set frame rate", cxxopts::value<int>()->default_value("30")},
       {"c,codec-type", "Codec type", cxxopts::value<std::string>()->default_value("hevc")},
       {"color-format", "Color format", cxxopts::value<std::string>()},
       {"chroma-format", "Chroma format", cxxopts::value<std::string>()->default_value("yuv420")},
       {"use-hw", "Use hardware implementation", cxxopts::value<bool>()->default_value("false")},
       {"help", "Print usage"}});
  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    return 0;
  }
  const bool use_hw_impl = result["use-hw"].as<bool>();
  const int frame_height = result["height"].as<int>();
  const int frame_width = result["width"].as<int>();
  const int frame_rate = result["rate"].as<int>();
  const std::string input_filename = result["input"].as<std::string>();
  const std::string output_filename = result["output"].as<std::string>();
  const auto codec_type = codec_formats.at(result["codec-type"].as<std::string>());
  const auto chroma_format = chroma_formats.at(result["chroma-format"].as<std::string>());

  // Setup input and output files
  std::ifstream input_file{input_filename, std::ios_base::in | std::ios_base::binary};
  if (!input_file) {
    std::cout << "Couldn't open input file" << std::endl;
    return 1;
  }

  std::ofstream output_file{output_filename, std::ios_base::out | std::ios_base::binary};
  if (!output_file) {
    std::cout << "Couldn't open output file" << std::endl;
    return 1;
  }

  int frame_num = 0;
  bool is_stillgoing = true;
  vpl::implementation_type impl_type{use_hw_impl ? vpl::implementation_type::hw
                                                 : vpl::implementation_type::sw};

  // Initialize VPL session for any implementation of HEVC/H265 encode
  // Default implementation selector. Selects first impl based on property list.
  vpl::default_selector impl_sel{{vpl::dprops::impl(impl_type),
                                  vpl::dprops::api_version(2, 5),
                                  vpl::dprops::encoder({vpl::dprops::codec_id(codec_type)})}};

  vpl::ExtDecodeErrorReport err_report{};
  vpl::color_format_fourcc input_fourcc = (impl_type == vpl::implementation_type::sw)
                                              ? vpl::color_format_fourcc::i420
                                              : vpl::color_format_fourcc::nv12;
  if (result.count("color-format")) {
    input_fourcc = color_formats.at(result["color-format"].as<std::string>());
  }

  // create raw freames reader
  vpl::raw_frame_file_reader reader(frame_height, frame_width, input_fourcc, input_file);

  // Load session and initialize encoder
  auto encoder = std::make_shared<vpl::encode_session>(impl_sel, &reader);

  // Initialize encode parameters
  auto enc_params = std::make_shared<vpl::encoder_video_param>();
  vpl::frame_info info{};

  info.set_frame_rate({frame_rate, 1});
  info.set_frame_size({ALIGN16(frame_height), ALIGN16(frame_width)});
  info.set_FourCC(input_fourcc);
  info.set_ChromaFormat(chroma_format);
  info.set_ROI({{0, 0}, {frame_height, frame_width}});
  info.set_PicStruct(vpl::pic_struct::progressive);

  enc_params->set_RateControlMethod(vpl::rate_control_method::cqp);
  enc_params->set_frame_info(std::move(info));
  enc_params->set_CodecId(codec_type);
  enc_params->set_IOPattern((kUseVideoMemory) ? vpl::io_pattern::in_device_memory
                                              : vpl::io_pattern::in_system_memory);

  try {
    encoder->Init(enc_params.get());
  } catch (vpl::base_exception& e) {
    std::cout << "Encoder init failed: " << e.what() << std::endl;
    return -1;
  }

  std::cout << info << std::endl;
  std::cout << "Init done" << std::endl;
  std::cout << "Encoding " << input_filename << " -> " << output_filename << std::endl;

  // main encoder Loop
  while (is_stillgoing == true) {
    vpl::status wrn = vpl::status::Ok;

    auto bitstream = std::make_shared<vpl::bitstream_as_dst>();
    try {
      wrn = encoder->encode_frame(bitstream);
    } catch (vpl::base_exception& e) {
      std::cout << "Encoder died: " << e.what() << std::endl;
      return -1;
    }

    switch (wrn) {
    case vpl::status::Ok: {
      std::chrono::duration<int, std::milli> waitduration(kWait100Ms);
      bitstream->wait_for(waitduration);
      writeEncodedStream(bitstream, &output_file);
      frame_num++;
    } break;
    case vpl::status::EndOfStreamReached:
      std::cout << "EndOfStream Reached" << std::endl;
      is_stillgoing = false;
      break;
    case vpl::status::DeviceBusy:
      // For non-CPU implementations,
      // Wait a few milliseconds then try again
      std::cout << "DeviceBusy" << std::endl;
      break;
    default:
      std::cout << "unknown status: " << static_cast<int>(wrn) << std::endl;
      is_stillgoing = false;
      break;
    }
  }

  std::cout << "Encoded " << frame_num << " frames" << std::endl;

  std::cout << "\n-- Encode information --\n\n";
  std::shared_ptr<vpl::encoder_video_param> p = encoder->working_params();
  std::cout << *(p.get()) << std::endl;

  return 0;
}

// Write encoded stream to file
