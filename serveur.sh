#!/bin/bash

#JULES 173
# MOI 247


if [ "$(uname)" == "Darwin" ]; then
		gst-launch-1.0 rtpbin name=rtpbin avfvideosrc capture-screen=true ! videorate ! videoscale ! videoconvert ! 'video/x-raw, format=(string)AYUV, width=(int)500, height=(int)500, framerate=(fraction)30/1' !  rtpvrawpay ! rtpbin ! udpsink clients="192.168.43.173:9996" sync=false async=false udpsrc port=10000 
elif [ "$(uname)" == "Linux" ]; then
	gst-launch-1.0 rtpbin name=rtpbin ximagesrc ! videorate ! videoscale ! videoconvert ! 'video/x-raw, format=(string)AYUV, width=(int)500, height=(int)500, framerate=(fraction)30/1' !  rtpvrawpay ! rtpbin ! udpsink clients="192.168.43.247:9996" sync=false async=false udpsrc port=10000 
fi;
