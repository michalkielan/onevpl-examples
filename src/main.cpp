// SPDX-License-Identifier: MIT

#include <chrono>
#include <iostream>

#include "cxxopts.hpp"
#include "nlohmann/json.hpp"
#include "vpl/preview/vpl.hpp"

#include "mapping.hpp"
#include "statistics.hpp"

#define ALIGN16(value) (((value + 15) >> 4) << 4)

constexpr const int kTimeout100Ms = 100;
constexpr const bool kUseVideoMemory = false;

namespace vpl = oneapi::vpl;

static void write_encoded_stream(std::shared_ptr<vpl::bitstream_as_dst> bits,
                                 std::ofstream* encoded_file) {
  auto [ptr, len] = bits->get_valid_data();
  encoded_file->write(reinterpret_cast<char*>(ptr), len);
  bits->set_DataLength(0);
  return;
}

static long time_since_epoch() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

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
    return ENOENT;
  }

  std::ofstream output_file{output_filename, std::ios_base::out | std::ios_base::binary};
  if (!output_file) {
    std::cout << "Couldn't open output file" << std::endl;
    return ENOENT;
  }

  bool is_stillgoing = true;
  std::vector<FrameInfo> frames_info;
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

  // Statistics data frame
  StatsDataFrame stats_data_frame{};
  stats_data_frame.settings.codec = result["codec-type"].as<std::string>();
  stats_data_frame.settings.gop = -1;
  stats_data_frame.settings.fps = frame_rate;
  stats_data_frame.settings.bitrate = "";
  stats_data_frame.settings.mean_bitrate = "";
  stats_data_frame.settings.width = frame_width;
  stats_data_frame.settings.height = frame_height;

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
    return EIO;
  }

  std::cout << info << std::endl;
  std::cout << "Init done" << std::endl;
  std::cout << "Encoding " << input_filename << " -> " << output_filename << std::endl;

  const auto encoding_start_time = time_since_epoch();
  // main encoder Loop
  while (is_stillgoing == true) {
    FrameInfo frame_info{};
    vpl::status wrn = vpl::status::Ok;

    auto bitstream = std::make_shared<vpl::bitstream_as_dst>();
    try {
      frame_info.start_time = time_since_epoch();
      wrn = encoder->encode_frame(bitstream);
      frame_info.stop_time = time_since_epoch();
    } catch (vpl::base_exception& e) {
      std::cout << "Encoder died: " << e.what() << std::endl;
      return EIO;
    }

    switch (wrn) {
    case vpl::status::Ok: {
      std::chrono::duration<int, std::milli> timeout(kTimeout100Ms);
      bitstream->wait_for(timeout);
      frame_info.size = bitstream->get_DataLength();
      write_encoded_stream(bitstream, &output_file);
      frame_info.iframe = (bitstream->get_FrameType() & MFX_FRAMETYPE_I) ? 1 : 0;
      frame_info.counter = stats_data_frame.frame_info.size();
      stats_data_frame.frame_info.emplace_back(std::move(frame_info));
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
  const auto encoding_end_time = time_since_epoch();
  stats_data_frame.id = "42";
  stats_data_frame.description = "onevpl encoder test";
  stats_data_frame.test = "test encoder parameters";
  stats_data_frame.test_definition = "n/a";
  stats_data_frame.date = "today";
  stats_data_frame.encapp_version = "1.6";
  stats_data_frame.proctime = encoding_end_time - encoding_start_time;
  stats_data_frame.framecount = frames_info.size();
  stats_data_frame.encoded_file = output_filename;
  stats_data_frame.source_file = input_filename;

  std::cout << "Encoded " << frames_info.size() << " frames" << std::endl;

  std::cout << "\n-- Encode information --\n\n";
  std::shared_ptr<vpl::encoder_video_param> video_param = encoder->working_params();
  std::cout << *(video_param.get()) << std::endl;
  Statistics stats{std::move(stats_data_frame)};
  stats.write("out.json");
  return 0;
}
