// SPDX-License-Identifier: MIT

#pragma once

#include <map>

#include "vpl/preview/vpl.hpp"

extern const std::map<std::string, oneapi::vpl::codec_format_fourcc> codec_formats;

extern const std::map<std::string, oneapi::vpl::chroma_format_idc> chroma_formats;

extern const std::map<std::string, oneapi::vpl::color_format_fourcc> color_formats;

extern const std::map<std::string, oneapi::vpl::rate_control_method> bitrate_control_method;
