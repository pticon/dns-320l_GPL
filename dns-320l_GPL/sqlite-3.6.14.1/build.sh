#!/bin/sh
make clean ; make distclean
./configure --disable-tcl LDFLAGS=-s --host=arm-gnu-linux
make

$STRIP .libs/libsqlite3.so.0
# .libs/libsqlite3.so.0
