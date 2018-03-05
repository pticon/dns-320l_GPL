#!/bin/sh

PREFIX=$PWD/../
		
./configure --host=arm-gnu-linux --prefix=$PREFIX/xinst CPPFLAGS=-I$PREFIX/include

make clean;make;make install

