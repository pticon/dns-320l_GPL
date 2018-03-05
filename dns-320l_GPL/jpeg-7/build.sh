#!/bin/sh
make clean ; make distclean
./configure LDFLAGS=-s --host=arm-gnu-linux
make

$STRIP .libs/libjpeg.so.7
$STRIP .libs/djpeg
# .libs/libjpeg.so.7 , .libs/djpeg
