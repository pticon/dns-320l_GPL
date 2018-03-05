#!/bin/sh
make clean
make distclean
./configure --host=arm-linux --prefix=${PWD}/.install --enable-utf8
make
