#!/bin/sh

./configure --prefix=$PWD/xinst --host=arm-linux --disable-nls --enable-elf-shlibs --disable-testio-debug

make
make install
