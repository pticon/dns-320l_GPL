#!/bin/sh

./configure --prefix=$PWD/xinst --host=arm-linux --disable-dependency-tracking --disable-nls

make
make install
