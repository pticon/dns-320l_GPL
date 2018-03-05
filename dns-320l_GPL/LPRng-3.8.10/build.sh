#!/bin/sh
./configure --host=arm-linux --target=arm-linux --with-include-gettext=no --prefix=/usr/local/LPRng --enable-strip --enable-shared=yes --with-cc=arm-mv5sft-linux-gnueabi-gcc
cp libtool_628x libtool
make clean;make
		
