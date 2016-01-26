#!/bin/bash


gst-launch  udpsrc caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:4:4, depth=(string)8, width=(string)320, height=(string)240, colorimetry=(string)SMPTE240M, ssrc=(uint)1825678493, payload=(int)96, clock-base=(uint)4068866987, seqnum-base=(uint)24582" port=9996 ! queue ! rtpvrawdepay  ! queue ! ffmpegcolorspace ! autovideosink
