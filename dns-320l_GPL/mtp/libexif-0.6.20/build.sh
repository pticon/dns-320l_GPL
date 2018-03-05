#!/bin/sh
make clean ; make distclean
./configure --host=arm-gnu-linux
make

$STRIP libexif/.libs/libexif.so.12
# libexif/.libs/libexif.so.12
