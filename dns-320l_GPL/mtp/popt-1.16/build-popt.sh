#!/bin/sh
make clean ; make distclean
./configure --host=arm-gnu-linux
make

$STRIP .libs/libpopt.so.0
# .libs/libpopt.so.0
