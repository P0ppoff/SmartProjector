#!/bin/bash

#Changer la source avec v4l2src pour la webcam.

gst-launch-1.0 -v rtpbin name=rtpbin ximagesrc ! videorate ! videoscale ! videoconvert ! 'video/x-raw, width=(int)500, height=(int)500, framerate=(fraction)15/1' !  rtpvrawpay ! rtpbin.send_rtp_sink_0 rtpbin.send_rtp_src_0 ! multiudpsink clients="127.0.0.1:9996" rtpbin.send_rtcp_src_0 ! multiudpsink clients="127.0.0.1:9997" sync=false async=false udpsrc port=10000 





