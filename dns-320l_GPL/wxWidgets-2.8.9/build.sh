#!/bin/sh

PREFIX=$PWD/../

./configure --host=arm-gnu-linux --prefix=$PREFIX/xinst --enable-unicode --disable-gui --enable-optimize --enable-shared CPPFLAGS=-I$PREFIX/include

make clean;make;make install
