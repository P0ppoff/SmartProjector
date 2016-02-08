#!/bin/bash

param="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW,"
param+="sampling=(string)YCbCr-4:4:4, depth=(string)8, width=(string)500, height=(string)500,colorimetry=(string)SMPTE240M,"
param+="ssrc=(uint)1825678493, payload=(int)96, clock-base=(uint)4068866987, seqnum-base=(uint)24582"


if [ "$(uname)" == "Darwin" ]; then
	gst-launch-1.0 udpsrc caps="$param" port=9996 ! queue ! rtpvrawdepay  ! queue ! videoconvert ! autovideosink
elif [ "$(uname)" == "Linux" ]; then
	gst-launch-1.0 udpsrc caps="$param" port=9996 ! queue ! rtpvrawdepay  ! queue ! videoconvert ! autovideosink
fi;

