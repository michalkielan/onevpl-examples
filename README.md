[![Build](https://github.com/michalkielan/onevpl-examples/actions/workflows/build.yml/badge.svg)](https://github.com/michalkielan/onevpl-examples/actions/workflows/build.yml)
# onevpl examples
> Video processing examples using onevpl library.

## Requirements
onevpn api and one of the implementation.
https://github.com/oneapi-src/oneVPL

### Install onevpl api from source
```
$ git clone https://github.com/oneapi-src/oneVPL/ onevpl
$ cd onevpl
$ mkdir build && cd build
$ cmake ..
$ make
$ sudo make install
```

### Install onevpl intel gpu implementation from source.
```
$ git clone https://github.com/oneapi-src/oneVPL-intel-gpu onevpl-gpu
$ cd onevpl-gpu
$ mkdir build && cd build
$ cmake ..
$ make
$ sudo make install
```

## Build examples
```
$ make
$ ./hello-encode
    BitDepthLuma   = 0
    BitDepthChroma = 0
    Shift          = Not Specifyed
    Color Format   = NV12
    Size [W,H]     = [320,240]
    ROI [X,Y,W,H]  = [0,0,320,240]
    FrameRate [N:D]= 30:1
    AspecRato [W,H]= [Unset]
    PicStruct      = Progressive Picture
    ChromaFormat   = 4:2:0

Init done
Encoding cars_320x240.i420 -> out.h265
EndOfStream Reached
Encoded 30 frames

-- Encode information --

Base:
    AllocId    = 0
    AsyncDepth = 5
    Protected  = 0
    IOPattern  = In System Memory
Codec:
    LowPower           = OFF
    BRCParamMultiplier = 1
    CodecId            = HEVC
    CodecProfile       = 1
    CodecLevel         = 20
    NumThread          = 0
FrameInfo:
    BitDepthLuma   = 8
    BitDepthChroma = 8
    Shift          = Not Specifyed
    Color Format   = NV12
    Size [W,H]     = [320,240]
    ROI [X,Y,W,H]  = [0,0,320,240]
    FrameRate [N:D]= 30:1
    AspecRato [W,H]= [1,1]
    PicStruct      = Progressive Picture
    ChromaFormat   = 4:2:0

Encoder:
    TargetUsage       = 4
    GopPicSize        = 65535
    GopRefDist        = 8
    GopOptFlag        = Value is out of possible values
    IdrInterval       = 0
    RateControlMethod = CQP
    InitialDelayInKB  = 26
    QPI               = 26
    Accuracy          = 26
    BufferSizeInKB    = 115
    TargetKbps        = 28
    QPP               = 28
    ICQQuality        = 28
    MaxKbps           = 30
    QPB               = 30
    Convergence       = 30
    NumSlice          = 1
    NumRefFrame       = 4
    EncodedOrder      = False
```
