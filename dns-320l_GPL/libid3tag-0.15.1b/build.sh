#!/bin/sh
echo make zlib first!
make clean ; make distclean
./configure CFLAGS=-I../zlib-1.2.3/ LDFLAGS=-L../zlib-1.2.3/ --host=arm-gnu-linux
make

$STRIP .libs/libid3tag.so.0
# .libs/libid3tag.so.0
