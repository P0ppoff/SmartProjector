#!/bin/bash

#Pour la source windows : dx9screencapsrc


if [ "$(uname)" == "Darwin" ]; then
	gst-launch-1.0 rtpbin name=rtpbin \
	avfvideosrc capture-screen=true \
		! videorate ! videoscale ! videoconvert \
		! 'video/x-raw, format=(string)AYUV, width=(int)500, height=(int)500, framerate=(fraction)30/1' \
		!  rtpvrawpay ! rtpbin.send_rtp_sink_0 rtpbin.send_rtp_src_0 \
		! multiudpsink clients="127.0.0.1:9996" rtpbin.send_rtcp_src_0 \
		! multiudpsink clients="127.0.0.1:9997" sync=false async=false udpsrc port=10000 
elif [ "$(uname)" == "Linux" ]; then
	gst-launch-1.0 rtpbin name=rtpbin \
	ximagesrc \
		! videorate ! videoscale ! videoconvert \
		! 'video/x-raw, format=(string)AYUV, width=(int)500, height=(int)500, framerate=(fraction)30/1' \
		!  rtpvrawpay ! rtpbin.send_rtp_sink_0 rtpbin.send_rtp_src_0 \
		! multiudpsink clients="127.0.0.1:9996" rtpbin.send_rtcp_src_0 \
		! multiudpsink clients="127.0.0.1:9997" sync=false async=false udpsrc port=10000 
fi;


