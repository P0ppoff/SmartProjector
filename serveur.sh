#!/bin/bash

# Celui qui reçoit les fluxs

if [ "$(uname)" == "Darwin" ]; then
	gst-launch-1.0 udpsrc \
		caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:4:4, depth=(string)8, width=(string)500, height=(string)500, colorimetry=(string)SMPTE240M, ssrc=(uint)1825678493, payload=(int)96, clock-base=(uint)4068866987, seqnum-base=(uint)24582" \
		port=9996 ! queue ! rtpvrawdepay  ! queue ! videoconvert ! osxvideosink	
elif [ "$(uname)" == "Linux" ]; then
		gst-launch-1.0  \
	videomixer name=mix  \
		    sink_1::xpos=0   sink_1::ypos=0 \
		    sink_2::xpos=600 sink_2::ypos=0 \
		    sink_3::xpos=0   sink_3::ypos=300 \
		    sink_4::xpos=600 sink_4::ypos=300 \
		! autovideosink \
		sync=false \
	v4l2src \
		! videoconvert ! videoscale \
		! video/x-raw ,width=600,height=300, framerate=30/1 \
		! mix.sink_1 \
	videotestsrc \
		! videoconvert ! videoscale \
		! video/x-raw ,width=600,height=300, framerate=30/1 \
		! mix.sink_2 \
	ximagesrc \
		! videoconvert ! videoscale \
		! video/x-raw ,width=600,height=300, framerate=30/1 \
		! mix.sink_3 \
	udpsrc port=5100 caps = "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)VP8, payload=(int)96, ssrc=(uint)2432391931, timestamp-offset=(uint)1048101710, seqnum-offset=(uint)9758" ! rtpvp8depay ! vp8dec  ! videoscale ! video/x-raw ,width=600,height=300\
		! mix.sink_4 
fi;



<<COMMENT1
RECEPTION ET AFFICHAGE DE PLUSIEURS ECRANS

	gst-launch-1.0 udpsrc port=5100 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)VP8-DRAFT-IETF-01, payload=(int)96, ssrc=(uint)2990747501, clock-base=(uint)275641083, seqnum-base=(uint)34810" ! rtpvp8depay ! vp8dec ! autovideosink
	# textoverlay font-desc="Sans 24" text="CAM3" valign=top halign=left shaded-background=true ! \
	
	#REMPLACER UN AUTRE SINK
		udpsrc port=5101 caps="application/x-rtp,width=600,height=300, framerate=30/1, media=(string)video, clock-rate=(int)90000, encoding-name=(string)VP8-DRAFT-IETF-01, payload=(int)96, ssrc=(uint)2990747501, clock-base=(uint)275641083, seqnum-base=(uint)34810" ! rtpvp8depay ! vp8dec ! videoscale \

		
COMMENT1
