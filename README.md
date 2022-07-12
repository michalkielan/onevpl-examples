[![Build](https://github.com/michalkielan/onevpl-examples/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/michalkielan/onevpl-examples/actions/workflows/build.yml?query=branch%3Amaster)
# onevpl examples
> Video processing examples using onevpl library.

## Requirements
onevpl api and one of the implementation.
https://github.com/oneapi-src/oneVPL

### Install onevpl api from source
```
$ git clone https://github.com/oneapi-src/oneVPL onevpl
$ cd onevpl
$ mkdir build && cd build
$ cmake ..
$ make
$ sudo make install
```

### Install onevpl cpu implementation from source.
```
$ git clone https://github.com/oneapi-src/oneVPL-cpu onevpl-cpu
$ cd onevpl-cpu
$ source script/bootstrap
$ script/build
$ sudo script/install
```

## Build examples
```
$ mkdir build && cd build
$ cmake ..
$ make
```

## Run examples
```
$ python hello_encode.py
$ ./build/encodeapp -i cars_320x240.i420 -w 640 -h 480 -c hevc
```

## HD video encoding
Run hevc encoder 720p file using onevpl-cpu.
```
$ wget https://media.xiph.org/video/derf/y4m/KristenAndSara_1280x720_60.y4m
$ ffmpeg -i KristenAndSara_1280x720_60.y4m -f rawvideo -pix_fmt yuv420p KristenAndSara_1280x720_60.yuv
$ ./hello_encode -i KristenAndSara_1280x720_60.yuv -h 1280 -w 720 -c hevc -r 60
    BitDepthLuma   = 0
    BitDepthChroma = 0
    Shift          = Not Specifyed
    Color Format   = IYUV
    Size [W,H]     = [1280,720]
    ROI [X,Y,W,H]  = [0,0,1280,720]
    FrameRate [N:D]= 60:1
    AspecRato [W,H]= [Unset]
    PicStruct      = Progressive Picture
    ChromaFormat   = 4:2:0

Init done
Encoding KristenAndSara_1280x720_60.yuv -> KristenAndSara_1280x720_60.hevc
EndOfStream Reached
Encoded 601 frames

-- Encode information --

Base:
    AllocId    = 0
    AsyncDepth = Not Specifyed
    Protected  = 0
    IOPattern  = In System Memory
Codec:
    LowPower           = Unset
    BRCParamMultiplier = 0
    CodecId            = HEVC
    CodecProfile       = 1
    CodecLevel         = 0
    NumThread          = 0
FrameInfo:
    BitDepthLuma   = 8
    BitDepthChroma = 8
    Shift          = Not Specifyed
    Color Format   = IYUV
    Size [W,H]     = [1280,720]
    ROI [X,Y,W,H]  = [0,0,1280,720]
    FrameRate [N:D]= 60:1
    AspecRato [W,H]= [1,1]
    PicStruct      = Progressive Picture
    ChromaFormat   = 4:2:0

Encoder:
    TargetUsage       = 4
    GopPicSize        = Not Specifyed
    GopRefDist        = Not Specifyed
    GopOptFlag        = Value is out of possible values
    IdrInterval       = 0
    RateControlMethod = CQP
    InitialDelayInKB  = 0
    QPI               = 0
    Accuracy          = 0
    BufferSizeInKB    = 0
    TargetKbps        = 0
    QPP               = 0
    ICQQuality        = 0
    MaxKbps           = 0
    QPB               = 0
    Convergence       = 0
    NumSlice          = Not Specifyed
    NumRefFrame       = Not Specifyed
    EncodedOrder      = False
```
