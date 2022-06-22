# pylint: disable=no-member
# ==============================================================================
#  Copyright Intel Corporation
#
#  SPDX-License-Identifier: MIT
# ==============================================================================
#
#  A minimal oneAPI Video Processing Library (oneVPL) encode application,
#  using 2.x API with internal memory management
"""
Example:  hello_encode
To view:  ffplay out.h265

* Encode raw frames to HEVC/H265 elementary stream in out.h265
  CPU native color format is I420/yuv420p.  GPU native color format is NV12
"""

import math
import sys
import pyvpl


INPUT_FILENAME = "cars_320x240.i420"
ENCODED_FILENAME = "out.h265"
WIDTH = 320
HEIGHT = 240


def verify(epectation, msg):
    """Check for expected condition and exit if not met"""
    if not epectation:
        print(msg)
        sys.exit(1)


def roundup(value, base=10):
    """Round value up to next multiple of base"""
    result = math.floor((value + base - 1) / base) * base
    return result


def main():
    """Main example"""
    frame_count = 0
    opts = pyvpl.properties()
    opts.api_version = (2, 5)
    opts.impl = pyvpl.implementation_type.hw
    input_fourcc = pyvpl.color_format_fourcc.nv12
    opts.encoder.enc_profile.enc_mem_desc.color_format = input_fourcc
    opts.encoder.codec_id = [pyvpl.codec_format_fourcc.hevc]
    sel_default = pyvpl.default_selector(opts)

    with pyvpl.raw_frame_file_reader_by_name(WIDTH, HEIGHT,
                                             input_fourcc,
                                             INPUT_FILENAME) as source:
        with open(ENCODED_FILENAME, "wb") as sink:
            # Load session and initialize encoder
            session = pyvpl.encode_session(sel_default, source)

            params = pyvpl.encoder_video_param()
            info = pyvpl.frame_info()

            info.frame_rate = (30, 1)
            info.frame_size = (roundup(WIDTH, 16), roundup(HEIGHT, 16))
            info.FourCC = input_fourcc
            info.ChromaFormat = pyvpl.chroma_format_idc.yuv420
            info.ROI = ((0, 0), (WIDTH, HEIGHT))
            info.PicStruct = pyvpl.pic_struct.progressive

            params.RateControlMethod = pyvpl.rate_control_method.cqp
            params.frame_info = info
            params.CodecId = pyvpl.codec_format_fourcc.hevc
            params.IOPattern = pyvpl.io_pattern.in_system_memory

            # Getting VAAPI handle not supported in Python sample
            init_list = pyvpl.encoder_init_list()
            try:
                session.Init(params, init_list)
            except pyvpl.base_exception as err:
                print(f"Encoder init failed: {err}")
                return -1
            print(str(info))
            print("Init done")
            print(f"Encoding {INPUT_FILENAME} -> {ENCODED_FILENAME}")
            # check and report implementation details
            version = session.version
            verify(version.Major > 1,
                   "Sample requires 2.x API implementation, exiting")

            for bits in session:
                frame_count += 1
                sink.write(bits)

            print(f"Encoded {frame_count} frames")

            print("")
            print("-- Encode information --")
            print("")
            print(session.working_params())

    return 0


if __name__ == '__main__':
    sys.exit(main())
