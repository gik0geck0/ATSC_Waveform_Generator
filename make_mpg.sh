#!/bin/sh

if [[ -z $1 || -z $2 ]]
then
    echo "Usage:    make_mpg.sh <input_image> <output_video>"
fi

#ffmpeg -loop 1 -shortest -f image2 -i $1 -c:v mpeg2video -tune stillimage -strict experimental -b:a 192k $2
ffmpeg -i $1 -vcodec mpeg2video -f mpegts $2
