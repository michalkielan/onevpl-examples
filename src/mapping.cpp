// SPDX-License-Identifier: MIT

#include "mapping.hpp"

namespace vpl = oneapi::vpl;

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
