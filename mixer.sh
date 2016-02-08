#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
	gst-launch-1.0  \
	videomixer name=mix \
		    sink_1::xpos=0   sink_1::ypos=0 \
		    sink_2::xpos=600 sink_2::ypos=0 \
		    sink_3::xpos=0   sink_3::ypos=300 \
		    sink_4::xpos=600 sink_4::ypos=300 \
		! osxvideosink \
		sync=false \
	wrappercamerabinsrc mode=2  \
		! videoconvert ! videoscale \
		! video/x-raw ,width=600,height=300, framerate=30/1 \
		! mix.sink_1 \
	videotestsrc \
		! videoconvert ! videoscale \
		! video/x-raw ,width=600,height=300, framerate=30/1 \
		! mix.sink_2 \
	avfvideosrc capture-screen=true \
		! videoconvert ! videoscale \
		! video/x-raw,width=600,height=300, framerate=30/1 \
		! mix.sink_3 \
	videotestsrc pattern=4 \
		! videoconvert ! videoscale \
		! video/x-raw ,width=600,height=300, framerate=30/1 \
		! mix.sink_4 
elif [ "$(uname)" == "Linux" ]; then
	gst-launch-1.0  \
	videomixer name=mix \
		    sink_1::xpos=0   sink_1::ypos=0 \
		    sink_2::xpos=600 sink_2::ypos=0 \
		    sink_3::xpos=0   sink_3::ypos=300 \
		    sink_4::xpos=600 sink_4::ypos=300 \
		! xvimagesink \
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
	videotestsrc pattern=4 \
		! videoconvert ! videoscale \
		! video/x-raw ,width=600,height=300, framerate=30/1 \
		! mix.sink_4 
fi;
