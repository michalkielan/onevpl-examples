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

#include "vpl/preview/vpl.hpp"

#define ALIGN16(value) (((value + 15) >> 4) << 4)

constexpr const int kWait100Ms = 100;
constexpr const int kFrameRate = 30;
constexpr const char *kInputFilename = "cars_320x240.i420";
constexpr const int kWidth = 320;
constexpr const int kHeight = 240;
constexpr const bool kUseVideoMemory = false;
constexpr const char *kEncodedFileName = "out.h265";

namespace vpl = oneapi::vpl;

static void writeEncodedStream(std::shared_ptr<vpl::bitstream_as_dst> bits,
                               std::ofstream *fileStream) {
  auto [ptr, len] = bits->get_valid_data();
  fileStream->write(reinterpret_cast<char *>(ptr), len);
  bits->set_DataLength(0);
  return;
}

int main() {
  // Setup input and output files
  std::ifstream source{kInputFilename,
                       std::ios_base::in | std::ios_base::binary};
  if (!source) {
    std::cout << "Couldn't open input file" << std::endl;
    return 1;
  }

  std::ofstream sink{kEncodedFileName,
                     std::ios_base::out | std::ios_base::binary};
  if (!sink) {
    std::cout << "Couldn't open output file" << std::endl;
    return 1;
  }

  int frame_num = 0;
  bool is_stillgoing = true;
  vpl::implementation_type impl_type{vpl::implementation_type::hw};

  // Initialize VPL session for any implementation of HEVC/H265 encode
  // Default implementation selector. Selects first impl based on property list.
  vpl::default_selector impl_sel{
      {vpl::dprops::impl(impl_type), vpl::dprops::api_version(2, 5),
       vpl::dprops::encoder(
           {vpl::dprops::codec_id(vpl::codec_format_fourcc::hevc)})}};

  vpl::ExtDecodeErrorReport err_report{};
  vpl::color_format_fourcc input_fourcc =
      (impl_type == vpl::implementation_type::sw)
          ? vpl::color_format_fourcc::i420
          : vpl::color_format_fourcc::nv12;

  // create raw freames reader
  vpl::raw_frame_file_reader reader(kWidth, kHeight, input_fourcc, source);

  // Load session and initialize encoder
  auto encoder = std::make_shared<vpl::encode_session>(impl_sel, &reader);

  // Initialize encode parameters
  auto enc_params = std::make_shared<vpl::encoder_video_param>();
  vpl::frame_info info{};

  info.set_frame_rate({kFrameRate, 1});
  info.set_frame_size({ALIGN16(kWidth), ALIGN16(kHeight)});
  info.set_FourCC(input_fourcc);
  info.set_ChromaFormat(vpl::chroma_format_idc::yuv420);
  info.set_ROI({{0, 0}, {kWidth, kHeight}});
  info.set_PicStruct(vpl::pic_struct::progressive);

  enc_params->set_RateControlMethod(vpl::rate_control_method::cqp);
  enc_params->set_frame_info(std::move(info));
  enc_params->set_CodecId(vpl::codec_format_fourcc::hevc);
  enc_params->set_IOPattern((kUseVideoMemory)
                                ? vpl::io_pattern::in_device_memory
                                : vpl::io_pattern::in_system_memory);

  try {
    encoder->Init(enc_params.get());
  } catch (vpl::base_exception &e) {
    std::cout << "Encoder init failed: " << e.what() << std::endl;
    return -1;
  }

  std::cout << info << std::endl;
  std::cout << "Init done" << std::endl;
  std::cout << "Encoding " << kInputFilename << " -> " << kEncodedFileName
            << std::endl;

  // main encoder Loop
  while (is_stillgoing == true) {
    vpl::status wrn = vpl::status::Ok;

    auto bitstream = std::make_shared<vpl::bitstream_as_dst>();
    try {
      wrn = encoder->encode_frame(bitstream);
    } catch (vpl::base_exception &e) {
      std::cout << "Encoder died: " << e.what() << std::endl;
      return -1;
    }

    switch (wrn) {
    case vpl::status::Ok: {
      std::chrono::duration<int, std::milli> waitduration(kWait100Ms);
      bitstream->wait_for(waitduration);
      writeEncodedStream(bitstream, &sink);
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
