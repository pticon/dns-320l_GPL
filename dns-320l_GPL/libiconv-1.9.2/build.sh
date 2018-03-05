#!/bin/sh
make clean; make distclean
./configure --host=arm-gnu-linux --prefix=/etc --enable-extra-encodings
make
$STRIP lib/.libs/libiconv.so.2

#cp include/iconv.h ../include/
#cp libcharset/include/libcharset.h ../include/
# lib/.libs/libiconv.so.2
