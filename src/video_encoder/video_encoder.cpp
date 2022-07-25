// SPDX-License-Identifier: MIT

#include "video_encoder.hpp"

namespace vpl = oneapi::vpl;

constexpr const bool kUseVideoMemory = false;

VideoEncoder::VideoEncoder(vpl::implementation_selector& impl_sel,
                           vpl::frame_source_reader* frame_source) :
  encoder_{std::make_shared<vpl::encode_session>(impl_sel, frame_source)} {}

void VideoEncoder::init(vpl::frame_info frame_info,
                        vpl::codec_format_fourcc codec_type,
                        vpl::rate_control_method bitrate_mode,
                        vpl::encoder_init_list encoder_init_list) {
  auto enc_params = std::make_shared<vpl::encoder_video_param>();
  enc_params->set_RateControlMethod(bitrate_mode);
  enc_params->set_frame_info(std::move(frame_info));
  enc_params->set_CodecId(codec_type);
  enc_params->set_IOPattern((kUseVideoMemory) ? vpl::io_pattern::in_device_memory
                                              : vpl::io_pattern::in_system_memory);
  encoder_->Init(enc_params.get(), encoder_init_list);
}

vpl::status VideoEncoder::encode(std::shared_ptr<vpl::bitstream_as_dst> bitstream) {
  return encoder_->encode_frame(bitstream);
}

std::shared_ptr<vpl::encoder_video_param> VideoEncoder::get_working_params() {
  return encoder_->working_params();
}
