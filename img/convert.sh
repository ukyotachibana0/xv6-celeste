#!/bin/sh

for i in *.png; do
  ffmpeg -f rawvideo -pix_fmt rgb24 - -i $i \
    | xxd -i > $i.h
done
