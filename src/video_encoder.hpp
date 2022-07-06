// SPDX-License-Identifier: MIT

#include "vpl/preview/vpl.hpp"

class VideoEncoder {
 public:
  VideoEncoder(oneapi::vpl::implementation_selector& impl_sel,
               oneapi::vpl::frame_source_reader* frame_source);

  void init(oneapi::vpl::frame_info frame_info,
            oneapi::vpl::codec_format_fourcc codec_type,
            oneapi::vpl::color_format_fourcc input_fourcc,
            oneapi::vpl::rate_control_method bitrate_mode);

  oneapi::vpl::status encode(std::shared_ptr<oneapi::vpl::bitstream_as_dst> bitstream);

  std::shared_ptr<oneapi::vpl::encoder_video_param> get_working_params();

 private:
  std::shared_ptr<oneapi::vpl::encode_session> encoder_;
};
