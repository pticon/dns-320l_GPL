#!/bin/sh

unset CFLAGS
unset LDFLAGS

make clean ; make distclean
./configure --shared
make

$STRIP libz.so
# libz.so.1
